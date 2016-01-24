!
! main code : call the subroutine defined in lib.f90, and a C function
!

PROGRAM truc
	INTEGER :: y = 2, z = 3
	call blah
	PRINT *, acfunction(y, z)
END PROGRAM
