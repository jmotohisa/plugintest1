/*
	XCPSpecifications.h - XCode Plugin API
	Copyright 2005-2008 Damien Bobillot.
	Distributed under the GPL v3 licence.
*/

#import <Foundation/Foundation.h>

@class PBXTargetBuildContext;
@class PBXFileType;
@class PBXFileTypePart;
@class PBXLexicalRules;
@class PBXSourceLexer;
@class PBXSourceScanner;

/*
	Generic specification object.
	
	Specification are automatically created by Xcode. However Xcode create a proxy version in
	order to load the sepcification only when first used : you must call -loadedSpecification to
	retreive the loaded specification object.
*/
@interface XCSpecification : NSObject
{
	NSString*				_identifier;
	XCSpecification*		_superSpecification;
#if XCODE_VERSION < 23
	NSMutableDictionary*	_properties;
#else
	NSDictionary*			_properties;
#endif
	NSDictionary*			_localizationDictionary;
	NSBundle*				_bundle;
	void*					_hackcatchingbug[1]; // hack to allow plugins to work with Mac OS X 10.5.5
}

/* Informations about this specification (class: XCSpecification, type: @"Generic",
path extensions: .pbsomethingspec...) */
+ (Class)specificationTypeBaseClass;
+ (NSString*)specificationType;
+ (NSString*)localizedSpecificationTypeName;
+ (NSSet*)specificationTypePathExtensions;

// Get specifications of this type
+ (NSDictionary*)specificationRegistry; // keys are identifiers
+ (XCSpecification*)specificationForIdentifier:(NSString*)identifier;
+ (XCSpecification*)specificationsForIdentifiers:(NSArray*)identifiers;
+ (NSArray*)registeredSpecifications;
+ (NSArray*)registeredBaseSpecifications;

// Get specifications of all types
+ (NSArray*)allRegisteredSpecifications;

// Loading
- (XCSpecification*)loadedSpecification;
- (BOOL)isNotYetLoadedSpecificationProxy;
- (BOOL)isMissingSpecificationProxy;

// Accessors
- (XCSpecification*)superSpecification;
- (NSArray*)subSpecifications;
- (BOOL)isKindOfSpecification:(XCSpecification*)specification;
- (NSString*)identifier;
- (NSString*)name;
- (NSString*)description;
- (NSString*)localizedDescription;
- (NSBundle*)bundle; // plugin bundle defining this specification
- (BOOL)isAbstract;
- (NSDictionary*)properties;
- (NSDictionary*)localizationDictionary;
#if XCODE_VERSION >= 30
- (id)type;
#endif

// Property list content
- (id)objectForKey:(NSString*)key;
- (NSString*)stringForKey:(NSString*)key;
- (NSArray*)arrayForKey:(NSString*)key;
- (NSDictionary*)dictionaryForKey:(NSString*)key;
- (NSData*)dataForKey:(NSString*)key;
- (int)integerForKey:(NSString*)key;
- (long long)longLongForKey:(NSString*)key;
- (float)floatForKey:(NSString*)key;
- (double)doubleForKey:(NSString*)key;
- (BOOL)boolForKey:(NSString*)key;
- (BOOL)boolForKeyFromProxy:(NSString*)key;
#if XCODE_VERSION >= 23
- (id)arrayOrStringForKey:(NSString*)key;
#endif

// Others (you don't need them)
+ (void)registerSpecificationTypeBaseClass:(Class)class;
+ (void)registerSpecificationOrProxy:(id)spec;
+ (id)registerSpecificationProxyFromPropertyList:(id)plist;
+ (id)registerSpecificationProxiesFromPropertyListsInDirectory:(id)fp8 recursively:(BOOL)fp12 inBundle:(id)fp16;
+ (id)registerSpecificationProxiesFromPropertyListsInDirectory:(id)fp8 recursively:(BOOL)fp12;
#if XCODE_VERSION >= 30
+ (id)specificationTypeForPathExtension:(id)fp8;
+ (Class)specificationBaseClassForType:(id)fp8;
#endif
- (id)initWithPropertyListDictionary:(id)plist;
- (id)initAsMissingSpecificationProxyWithIdentifier:(id)fp8 name:(id)fp12 description:(id)fp16;
- (int)identifierCompare:(id)fp8;
- (int)nameCompare:(id)fp8;
@end

/*
	XCSpecification using build settings
*/
@interface XCPropertyDomainSpecification : XCSpecification
{
	NSDictionary*			_buildOptions;
	NSArray*				_orderedBuildOptions;
	NSArray*				_optionNamesForCommandLine;
	NSArray*				_commonBuildOptions;
#if XCODE_VERSION < 30
	NSArray*				_buildOptionCategories;
#else
    NSSet*					_buildOptionConditionFlavors;
#endif
	XCPropertyDomainSpecification* _specForUserInterface;
	NSDictionary*			_flattenedBuildOptions;
	NSArray*				_flattenedOrderedBuildOptions;
	NSArray*				_flattenedCommonBuildOptions;
	NSArray*				_flattenedOptionNamesForCommandLine;
	NSArray*				_flattenedOptionCategories;
	NSDictionary*			_flattenedDefaultValues;
}
#if XCODE_VERSION >= 30
+ (id)allRegisteredPropertyDomains;
+ (id)propertyDomainForIdentifier:(id)fp8;
#endif

// Accessors
- (NSDictionary*)buildOptions;	// "Options" key, as a dictionary : option name => option
#if XCODE_VERSION == 22
- (id)buildOptionForKey:(NSString*)key;
#elif XCODE_VERSION >= 23
- (id)buildOptionNamed:(NSString*)name;
#endif
- (NSArray*)orderedBuildOptions; // Like buildOptions, but as an array
#if XCODE_VERSION < 30
- (NSArray*)commonBuildOptions; // Like orderedBuildOptions but only items whose "CommonOption" key is YES
#endif
- (NSArray*)namesOfBuildOptionsForCommandLine; // names of all options
- (NSArray*)namesOfFlattenedOptionsForCommandLine;
- (NSDictionary*)defaultValuesForAllOptions; // dictionary : option name => default value
- (id)buildOptionCategories; // "OptionCategories" key
- (NSArray*)flattenedOptionCategories;
#if XCODE_VERSION >= 30
- (id)buildOptionConditionFlavors;
#endif

- (XCPropertyDomainSpecification*)specificationToShowInUserInterface; // show options for this command into the GUI group of the returned command
- (NSArray*)commandLineForAutogeneratedOptionsForKey:(NSString*)key inTargetBuildContext:(PBXTargetBuildContext*)context;
@end

/*
	Command line tool specification
*/
@interface XCCommandLineToolSpecification : XCPropertyDomainSpecification
{
#if XCODE_VERSION >= 23 && XCODE_VERSION < 30
    Class					_commandInvocationClass;
#endif
#if XCODE_VERSION < 30
	NSArray*				_outputParserClassesOrRules;
#else
    id _tsIvars;
    id RESERVED;
#endif
}
+ (id)unionedDefaultValuesForAllPropertiesForAllTools;
#if XCODE_VERSION >= 30
+ (id)unionedDefaultValuesForAllPropertiesForAllTools_asPropertyValues;
#endif

// Accessors
- (NSString*)path;	// ExecPath key
#if XCODE_VERSION >= 23
- (Class)commandInvocationClass;
#endif
- (id)commandOutputParserClassesOrParseRules;
#if XCODE_VERSION >= 30
- (id)inputFileTypes;
- (BOOL)acceptsInputFileType:(id)fp8;
- (id)defaultDeploymentTarget;
- (void)setDefaultDeploymentTarget:(id)fp8;
#endif

// Auto computed argument array
- (NSArray*)commandLineForAutogeneratedOptionsInTargetBuildContext:(PBXTargetBuildContext*)context;

- (BOOL)areOutputFilesAffectedByCommandLineArgument:(id)fp8;
- (BOOL)areOutputFilesAffectedByEnvironmentVariable:(id)fp8;
- (id)instantiatedCommandOutputParserWithDelegate:(id)fp8;
#if XCODE_VERSION < 22
- (void)createInvocationInContext:(id)fp8;
#else
- (id)createCommandsInBuildContext:(id)fp8;
- (unsigned)concurrentExecutionCountInTargetBuildContext:(id)fp8;
#endif
#if XCODE_VERSION >= 23
- (id)hashStringForCommandLineComponents:(id)fp8 inputFilePaths:(id)fp12 inTargetBuildContext:(id)fp16;
#endif
#if XCODE_VERSION >= 30
- (id)arrayByRemovingArgumentsNotAffectingOutputFileFromArray:(id)fp8;
- (void)checkDeploymentTargetAgainstSDKInTargetBuildContext:(id)fp8;
#endif
@end

/*
	A Compiler definition (.pbcompspec)
*/
@interface XCCompilerSpecification : XCCommandLineToolSpecification
{
	NSMutableArray*			_inputFileTypes;
}
+ (id)displaySpecifications;

// Accessors
#if XCODE_VERSION < 30
- (id)inputFileTypes; // "FileTypes" key
- (BOOL)acceptsInputFileType:(id)type;
#endif
- (BOOL)isAbstract; // "Abstract" key
- (NSString*)builtinJambaseRuleName; // "BuiltinJambaseRuleName" key
#if XCODE_VERSION < 22
- (NSString*)jamfileCodeString;
#endif
- (NSString*)defaultOutputDirectory;
#if XCODE_VERSION >= 30
- (id)supportedArchitectures;
#endif

// Compiler tasks
#if XCODE_VERSION < 23
- (XCCompilerSpecification*)effectiveCompilerSpecificationForFileNamed:(NSString*)file inTargetBuildContext:(PBXTargetBuildContext*)context;
- (NSArray*)computeDependenciesForFilePaths:(NSArray*)paths outputDirectory:(NSString*)outputDir inTargetBuildContext:(PBXTargetBuildContext*)context;
#endif
- (NSString*)uniqueOutputBaseNameForInputFilePath:(NSString*)path inTargetBuildContext:(PBXTargetBuildContext*)context;
- (NSArray*)outputFilesForInputFilePath:(NSString*)path inTargetBuildContext:(PBXTargetBuildContext*)context;
#if XCODE_VERSION >= 22
- (PBXFileType*)adjustedFileTypeForInputFileAtPath:(NSString*)path originalFileType:(PBXFileType*)type inTargetBuildContext:(PBXTargetBuildContext*)context;
#endif
#if XCODE_VERSION >= 23
- (XCCompilerSpecification*)effectiveCompilerSpecificationInPropertyExpansionContext:(id)context;
#endif
#if XCODE_VERSION < 22
@end
@interface XCCompilerSpecification (PublicAPI)
#endif
- (NSString*)executablePath;
- (NSString*)defaultOutputDirectory;
- (NSArray*)computeDependenciesForInputFile:(NSString*)path ofType:(PBXFileType*)type variant:(NSString*)variant architecture:(NSString*)arch outputDirectory:(NSString*)outputDir inTargetBuildContext:(PBXTargetBuildContext*)context;
- (NSArray*)computeDependenciesForFilePath:(NSString*)path ofType:(PBXFileType*)type outputDirectory:(NSString*)outputDir inTargetBuildContext:(PBXTargetBuildContext*)context;
#if XCODE_VERSION < 23
- (XCCompilerSpecification*)effectiveCompilerSpecificationForFileNamed:(NSString*)file ofType:(PBXFileType*)type inTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)appendAdditionalLinkerArguments:(NSArray*)args forBuildOption:(NSString*)option inTargetBuildContext:(PBXTargetBuildContext*)context;
#endif
@end

@interface XCCompilerSpecification (IndexingSupport)
- (id)builtinQuoteSearchPathsInBuildContext:(id)fp8 forLanguageDialect:(id)fp12;
- (id)builtinBracketSearchPathsInBuildContext:(id)fp8 forLanguageDialect:(id)fp12;
- (id)builtinFrameworkSearchPathsInBuildContext:(id)fp8 forLanguageDialect:(id)fp12;
- (id)builtinMacroDefinitionsInBuildContext:(id)fp8 forLanguageDialect:(id)fp12;
@end

@interface XCCompilerSpecification (BDExtensions)
- (NSArray*)importedFilesForPath:(NSString*)path ensureFilesExist:(BOOL)ensure inTargetBuildContext:(PBXTargetBuildContext*)context;
@end

/*
	A Linker definition (.pblinkspec)
*/
@interface XCLinkerSpecification : XCCommandLineToolSpecification
{
	NSMutableArray*			_inputFileTypes;
}
+ (id)linkerForFileNamed:(id)file;

// Accesors
- (NSArray*)architectures;
- (NSArray*)inputFileTypes;
- (BOOL)acceptsFileNamed:(NSString*)name;
- (NSString*)executablePathInTargetBuildContext:(PBXTargetBuildContext*)context;
- (NSArray*)computeDependenciesForFilePaths:(NSArray*)paths outputPath:(NSString*)output inTargetBuildContext:(PBXTargetBuildContext*)context;
@end

/*
	A file description (.pbfilespec)	
*/
@interface PBXFileType : XCSpecification
{
	NSArray*				_extensions;
}
// Generic types
+ (PBXFileType*)genericFileType;
+ (PBXFileType*)textFileType;
+ (PBXFileType*)genericFolderType;
+ (PBXFileType*)wrapperFolderType;

// Compute file type from file informations
+ (PBXFileType*)fileTypeForPath:(NSString*)path;
#if XCODE_VERSION >= 30
+ (PBXFileType*)fileTypeUsingFileAttributesAtPath:(NSString*)path;
#endif
+ (PBXFileType*)fileTypeForPath:(NSString*)path getExtraFileProperties:(id*)props;
+ (PBXFileType*)fileTypeForFileName:(NSString*)name;
#if XCODE_VERSION < 22
+ (PBXFileType*)fileTypeForFileName:(NSString*)name posixPermissions:(unsigned)perms hfsTypeCode:(unsigned)type hfsCreatorCode:(unsigned)creator;
#else
+ (PBXFileType*)fileTypeForFileName:(NSString*)name posixPermissions:(unsigned)perms hfsTypeCode:(unsigned long)type hfsCreatorCode:(unsigned long)creator;
#endif
+ (PBXFileType*)guessFileTypeForGenericFileAtPath:(NSString*)path withFileAttributes:(id)attr getExtraFileProperties:(id*)props;
+ (PBXFileType*)bestFileTypeForRepresentingFileAtPath:(NSString*)path withFileAttributes:(id)attr withLessSpecificFileType:(PBXFileType*)type getExtraFileProperties:(id*)props;

// Accessors
- (id)extensions;
- (id)hfsTypeCodes;
- (BOOL)isBundle;
- (BOOL)isApplication;
- (BOOL)isLibrary;
- (BOOL)isDynamicLibrary;
- (BOOL)isStaticLibrary;
- (BOOL)isFramework;
- (BOOL)isStaticFramework;
- (BOOL)isProjectWrapper;
- (BOOL)isTargetWrapper;
- (BOOL)isExecutable;
- (BOOL)isExecutableWithGUI;
- (BOOL)isPlainFile;
- (BOOL)isTextFile;
- (BOOL)isSourceCode;
- (BOOL)isDocumentation;
- (BOOL)isFolder;
- (BOOL)isNonWrapperFolder;
- (BOOL)isWrapperFolder;
- (BOOL)includeInIndex;
- (BOOL)isTransparent;
- (BOOL)canSetIncludeInIndex;
#if XCODE_VERSION >= 22
- (BOOL)isScannedForIncludes;
#endif
- (NSString*)languageSpecificationIdentifier;
#if XCODE_VERSION >= 30
- (NSString*)xcLanguageSpecificationIdentifier;
#endif
- (BOOL)requiresHardTabs;
- (NSString*)description;

- (id)extraPropertyNames;
- (PBXFileTypePart*)subpathForWrapperPart:(NSString*)part ofPath:(NSString*)path withExtraFileProperties:(id)props;
- (PBXFileTypePart*)fileTypePartForIdentifier:(NSString*)identifier;
@end

/*
	Product type definition
*/
@protocol XCProductPartOwners
- (id)productPartForIdentifier:(id)fp8;
- (id)subpartsForProductPart:(id)fp8;
@end

@interface XCProductTypeSpecification : XCSpecification <XCProductPartOwners>
{
	NSDictionary*			_defaultBuildSettings;
	NSDictionary*			_flattenedDefaultBuildSettings;
#if XCODE_VERSION >= 30
    NSDictionary*			_flattenedDefaultSettingsAsProperties;
#endif
	NSSet*					_allowedBuildPhaseClasses;
	NSArray*				_packageTypes;
	NSArray*				_productParts;
}
// Accessors
- (NSString*)defaultTargetName;
- (id)defaultBuildSettings;
#if XCODE_VERSION >= 30
- (id)defaultBuildSettings_asPropertyValues;
#endif
- (id)allowedBuildPhaseClasses;
- (id)packageTypes;
- (id)defaultPackageType;
- (id)productParts;
- (id)productPartForIdentifier:(NSString*)identifier;
- (id)subpartsForProductPart:(NSString*)part;
- (BOOL)hasInfoPlist;
- (NSString*)iconNamePrefix;

// ?
- (void)computeDependenciesInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)initializeTemporaryBuildSettingsInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)initializeBuildSettingsInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)computeProductDependenciesInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)initializeSearchPathBuildSettingsInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)alterBuildSettingsToUseSDKPackageInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)defineAuxiliaryFilesInTargetBuildContext:(PBXTargetBuildContext*)context;
#if XCODE_VERSION < 22
- (void)computePostprocessingDependenciesInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)moveAsideRawLinkedProductInTargetBuildContext:(PBXTargetBuildContext*)context;
#endif
#if XCODE_VERSION >= 22
- (void)copyAsideProductInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)compileAuxiliaryFilesForVariant:(NSString*)variant architecture:(NSString*)arch inTargetBuildContext:(PBXTargetBuildContext*)context;
- (BOOL)shouldStripSymbolsOfLinkedProductInTargetBuildContext:(PBXTargetBuildContext*)context separately:(char*)fp12;
#endif
#if XCODE_VERSION >= 23
#if XCODE_VERSION < 30
- (void)generateDSYMFileForLinkedProductInTargetBuildContext:(PBXTargetBuildContext*)context;
#endif
- (id)createUniversalBinaryFromThinBinaries:(NSArray*)binaries inTargetBuildContext:(PBXTargetBuildContext*)context;
#endif
- (void)editSymbolsOfLinkedProductInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)ranlibLinkedProductInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)separatelyStripSymbolsOfLinkedProductInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)changePermissionsOnProductInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)computeSymlinkDependenciesInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)computeProductTouchActionInTargetBuildContext:(PBXTargetBuildContext*)context;
- (XCLinkerSpecification*)linkerSpecificationForObjectFilesInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)addBaseLinkerFlagsInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)addWarningLinkerFlagsInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)addInstallNameLinkerFlagsInTargetBuildContext:(PBXTargetBuildContext*)context;
- (void)addStripLinkerFlagsInTargetBuildContext:(PBXTargetBuildContext*)context;
@end

@protocol BDExtension_HeaderCopyPhase
- (void)computeDependenciesForHeaderFile:(NSString*)path ofType:(PBXFileType*)type inTargetBuildContext:(PBXTargetBuildContext*)context;
@end


/*
	Language specification
*/
@interface PBXLanguageSpecification : XCSpecification
{
	NSDictionary*			_syntaxColoringRules;
	NSDictionary*			_indentationRules;
	NSString*				_scannerClassName;
	Class					_scannerClass;
	NSString*				_lexerClassName;
	Class					_lexerClass;
	PBXLexicalRules*		_lexRules;
	BOOL					_supportsSyntaxAwareIndenting;
}
- (NSDictionary*)syntaxColoringRules;
- (NSDictionary*)indentationRules;
- (BOOL)supportsSyntaxAwareIndenting;
- (PBXSourceScanner*)sourceScanner;
- (PBXSourceLexer*)sourceLexer;
- (PBXLexicalRules*)loadBaseLexicalRules;
- (PBXLexicalRules*)lexicalRules;
@end

/**/
@interface PBXPackageTypeSpecification : XCSpecification
{
	NSDictionary*	_defaultBuildSettings;
	NSDictionary*	_flattenedDefaultBuildSettings;
#if XCODE_VERSION >= 30
    NSDictionary*	_flattenedDefaultSettingsAsProperties;
#endif
	NSString*		_productReferenceFileTypeIdentifier;
	PBXFileType*	_productReferenceFileType;
	NSString*		_productReferenceName;
	BOOL			_productReferenceIsLaunchable;
}
+ (id)wrapperSubpathForWrapperPart:(int)part;
- (id)defaultBuildSettings;
#if XCODE_VERSION >= 30
- (id)defaultBuildSettings_asPropertyValues;
#endif
- (id)productReferenceFileType;
- (id)productReferenceName;
- (BOOL)productReferenceIsLaunchable;
@end

@interface PBXRuntimeSystemSpecification : XCSpecification
{
}
+ (PBXRuntimeSystemSpecification*)nativeRuntimeSystemSpecificaton;
- (id)infoPlistKey;
- (id)specificResourcePath;
@end

@interface PBXBuildSettingsPaneSpecification : XCSpecification
{
	NSString*				_settingsDomainPath;
	NSString*				_paneClassName;
	NSString*				_paneFollows;
	NSArray*				_widgets;
}
+ (id)registeredSpecificationsSorted;
- (id)name;
- (id)settingsDomainPath;
- (id)paneClassName;
- (Class)paneClass;
- (id)paneFollows;
- (id)widgets;
@end

@interface PBXSCMSpecification : XCSpecification
{
	NSString*				_classBaseName;
	BOOL					_canAddDirectories;
	BOOL					_canRenameFiles;
#if XCODE_VERSION >=30
    NSString *_repositoryClassName;
#endif
}
- (NSString*)classBaseName;
#if XCODE_VERSION >=30
- (id)repositoryClassName;
- (Class)repositoryClass;
#endif
- (BOOL)canAddDirectories;
- (BOOL)canRenameFiles;
@end

@interface XCArchitectureSpecification : XCSpecification
{
#if XCODE_VERSION < 30
	unsigned				_byteOrder;
#else
    int						_byteOrder;
    NSString*				_canonicalName;
    NSArray*				_realArchitectures;
    NSString*				_architectureSetting;
#endif
}
#if XCODE_VERSION < 30
- (unsigned)byteOrder;
#else
- (long)byteOrder;
+ (id)validArchitectureCanonicalNames;
- (id)canonicalName;
- (id)realArchitectures;
- (id)architectureSetting;
#endif
@end

@interface XCPlatformSpecification : XCSpecification
{
	NSArray*				_architectures;
}
- (NSArray*)architectures;
@end

/*
@interface PBXPlistFileType : PBXFileType
@interface PBXHTMLFileType : PBXFileType
@interface PBXMachOFileType : PBXFileType
@interface PBXCFBundleWrapperFileType : PBXFileType
	@interface PBXFrameworkWrapperFileType : PBXCFBundleWrapperFileType
		@interface XCStaticFrameworkWrapperFileType : PBXFrameworkWrapperFileType
	@interface PBXApplicationWrapperFileType : PBXCFBundleWrapperFileType

@interface XCBuildSystemSpecification : XCPropertyDomainSpecification
@interface XCBuildPhaseSpecification : XCPropertyDomainSpecification

@interface XCCompilerSpecificationCpp : XCCompilerSpecification
@interface PBXCompilerSpecificationGcc : XCCompilerSpecification
	@interface PBXCompilerSpecificationGcc2_95_2 : PBXCompilerSpecificationGcc
		@interface PBXCompilerSpecificationGcc3_1 : PBXCompilerSpecificationGcc2_95_2
			@interface PBXCompilerSpecificationGcc3_3 : PBXCompilerSpecificationGcc3_1
@interface PBXCompilerSpecificationGccPlusPlus : XCCompilerSpecification
@interface PBXCompilerSpecificationPbxCp : XCCompilerSpecification
@interface PBXCompilerSpecificationMig : XCCompilerSpecification
@interface PBXCompilerSpecificationRez : XCCompilerSpecification
@interface PBXCompilerSpecificationLex : XCCompilerSpecification
@interface PBXCompilerSpecificationYacc : XCCompilerSpecification

@interface PBXLinkerSpecificationLd : XCLinkerSpecification
	@interface PBXLinkerSpecificationZeroLink : PBXLinkerSpecificationLd
@interface PBXLinkerSpecificationLibtool : XCLinkerSpecification
@interface PBXLinkerSpecificationResMerger : XCLinkerSpecification

@interface PBXBundleProductType : XCProductTypeSpecification
	@interface XCKernelExtensionProductType : PBXBundleProductType
	@interface PBXApplicationProductType : PBXBundleProductType
	@interface PBXFrameworkProductType : PBXBundleProductType
		@interface XCStaticFrameworkProductType : PBXFrameworkProductType
@interface XCStandaloneExecutableProductType : XCProductTypeSpecification
	@interface PBXToolProductType : XCStandaloneExecutableProductType
	@interface PBXDynamicLibraryProductType : XCStandaloneExecutableProductType
	@interface PBXStaticLibraryProductType : XCStandaloneExecutableProductType
@interface XCFolderProductType : XCProductTypeSpecification
@interface XCInfoPlistProductType : XCProductTypeSpecification
@interface XCMachOExecutableProductType : XCProductTypeSpecification
@interface XCSymLinkProductType : XCProductTypeSpecification
@interface XCWrapperProductType : XCProductTypeSpecification
*/
