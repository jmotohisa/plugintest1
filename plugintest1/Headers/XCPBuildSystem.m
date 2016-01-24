/*
	XCPBuildSystem.h - XCode Plugin API
	Copyright 2005-2008 Damien Bobillot.
	Distributed under the GPL v3 licence.
	
	Define a function to retreive all files in the framework build phase.
*/

#import "XCPBuildSystem.h"
#import "XCPSpecifications.h"
#import <objc/objc-class.h>
#import <Foundation/Foundation.h>

/*
	Some hidden declarations
*/

@interface PBXFileReference
- (NSArray*)referencesForBuilding;
- (NSString*)path;
- (NSString*)unexpandedAbsolutePath;
- (PBXFileType*)fileType;
@end

@interface PBXBuildFile
- (PBXFileReference*)fileReference;
@end

@interface PBXBuildPhase
#if XCODE_VERSION < 22
- (NSArray*)filteredBuildFilesForTargetBuildContext:(PBXTargetBuildContext*)context;
#else
- (NSArray*)buildFiles;
#endif
@end

@interface PBXTarget
- (PBXBuildPhase*)defaultFrameworksBuildPhase;
@end

@interface PBXTargetBuildContext (BDPrivate)
- (PBXTarget*)target;
@end

@interface XCHeadersBuildPhaseDGSnapshot : NSObject
- (void)computeDependenciesForBuildFileReference:(PBXFileReference*)ref inTargetBuildContext:(PBXTargetBuildContext*)context;
@end

/*
	linkedLibraryPaths : get files in the "link with library & framework" phase
*/
@implementation PBXTargetBuildContext (BDExtensions)
- (NSArray*)linkedLibraryPaths {
	// From reverse engineering of the Ld linker
	NSMutableArray* paths = [NSMutableArray arrayWithCapacity:20];
	
	PBXBuildPhase* buildPhase = [[self target] defaultFrameworksBuildPhase];
#if XCODE_VERSION < 22
	NSEnumerator* buildFileEnum = [[buildPhase filteredBuildFilesForTargetBuildContext:self] objectEnumerator];
#else
	NSEnumerator* buildFileEnum = [[buildPhase buildFiles] objectEnumerator];
#endif
	PBXBuildFile* buildFile;
	while((buildFile = [buildFileEnum nextObject]) != nil) {
	
		NSEnumerator* fileEnum = [[[buildFile fileReference] referencesForBuilding] objectEnumerator];
		PBXFileReference* file;
		while((file = [fileEnum nextObject]) != nil)
			[paths addObject:[file unexpandedAbsolutePath]];
	}
	
	return paths;
}

/*
	Hook -[PBXTargetBuildContext importedFilesForPath:ensureFilesExist:]
*/
typedef NSArray* (*importedFilesForPath_ensureFilesExist_func)(PBXTargetBuildContext* self, SEL _sel, NSString* path, BOOL ensure);
typedef NSArray* (*copyheader_compute_dependencies_func)(XCHeadersBuildPhaseDGSnapshot* self, SEL _sel, id file, PBXTargetBuildContext* context);
static importedFilesForPath_ensureFilesExist_func __original_importedFilesForPath_ensureFilesExist = nil;
static copyheader_compute_dependencies_func __original_copyheader_compute_dependencies = nil;
static NSMutableDictionary* __compilers_with_import = nil;
static NSMutableDictionary* __products_with_copyheaders = nil;


#if XCODE_VERSION < 30
// For Objective-C 1 runtime (Mac OS X 10.4 / Xcode 2.x).
IMP class_replaceMethod(Class class, SEL selector, IMP new_imp, const char* signature) {
	// Replace method
	Method m = class_getInstanceMethod(class, selector);
	IMP old_imp = m->method_imp;
	m->method_imp = new_imp;
	
	// Clear Objective-C runtime cache
	if(c->cache->mask != 0) memset(c->cache->buckets, 0, (c->cache->mask+1)*sizeof(Method));
	
	return old_imp;
}
#else
// For Objective-C 2 runtime (Mac OS X 10.5 / Xcode 3.x)
// Already exists in the runtime
#endif

NSArray* importedFilesForPath_ensureFilesExist_hook(PBXTargetBuildContext* self, SEL _sel, NSString* path, BOOL ensure)
{
	PBXFileType* type = [PBXFileType fileTypeForFileName:path];
	
	// Check if type is in our list
	NSEnumerator* e = [__compilers_with_import keyEnumerator];
	PBXFileType* t;
	while((t = [e nextObject]) != nil) {
		if(![type isKindOfSpecification:t]) continue;
		XCCompilerSpecification* spec = (XCCompilerSpecification*)[__compilers_with_import objectForKey:t];
		return [spec importedFilesForPath:path ensureFilesExist:ensure inTargetBuildContext:self];
	}
	
	// Type not found, call default code
	return __original_importedFilesForPath_ensureFilesExist(self, _sel, path, ensure);
}

+ (void)activateImportedFileType:(PBXFileType*)type withCompiler:(XCCompilerSpecification*)spec
{
	// Setup hook for importedFilesForPath:ensureFilesExist:
	if(__original_importedFilesForPath_ensureFilesExist == nil) {
		__compilers_with_import = [NSMutableDictionary new];

		__original_importedFilesForPath_ensureFilesExist =
			(importedFilesForPath_ensureFilesExist_func) class_replaceMethod(
				[self class],
				@selector(importedFilesForPath:ensureFilesExist:),
				(IMP)importedFilesForPath_ensureFilesExist_hook,
				"@16@0:4@8c12"
		);
	}
	
	// Add include parser for this file type
	[__compilers_with_import setObject:spec forKey:type];
}

void copyheader_compute_dependencies_hook(XCHeadersBuildPhaseDGSnapshot* self, SEL _sel, PBXFileReference* fileref, PBXTargetBuildContext* context)
{
	PBXFileType* type = [fileref fileType];
	
	// Check if type is in our list
	NSEnumerator* e = [__products_with_copyheaders keyEnumerator];
	PBXFileType* t;
	while((t = [e nextObject]) != nil) {
		if(![type isKindOfSpecification:t]) continue;
		id<BDExtension_HeaderCopyPhase> spec = (id<BDExtension_HeaderCopyPhase>)[__products_with_copyheaders objectForKey:t];
		[spec computeDependenciesForHeaderFile:[fileref path] ofType:type inTargetBuildContext:context];
		return;
	}
	
	// Type not found, call default code
	__original_copyheader_compute_dependencies(self, _sel, fileref, context);
}

+ (void)activateCopyHeaderFileType:(PBXFileType*)type withSpecification:(id<BDExtension_HeaderCopyPhase>)spec
{
	// Setup hook for importedFilesForPath:ensureFilesExist:
	if(__original_copyheader_compute_dependencies == nil) {
		__products_with_copyheaders = [NSMutableDictionary new];

		__original_copyheader_compute_dependencies =
			(copyheader_compute_dependencies_func) class_replaceMethod(
				[XCHeadersBuildPhaseDGSnapshot class],
				@selector(computeDependenciesForBuildFileReference:inTargetBuildContext:),
				(IMP)copyheader_compute_dependencies_hook,
				"v16@0:4@8@12"
		);
	}
	
	// Add include parser for this file type
	[__products_with_copyheaders setObject:spec forKey:type];
}
@end


@interface PBXFileType (BDExtensions)
- (id) copyWithZone:(NSZone*)zone;
@end 

@implementation PBXFileType (BDExtensions)
- (id) copyWithZone:(NSZone*)zone { return [self retain]; }
@end
