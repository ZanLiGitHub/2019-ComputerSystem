	@x
	M=0
	@sum
	M=0
(LOOP)
	@x
	D=M
	@10
	D=D-A
	@END
	D;JGE

	@numbers
	D=M
	@x
	A=D+M
	D=M

	@sum	
	M=M+D

	@x
	D=M+1

	@LOOP
	0,JMP
(END)
	@END
	0;JMP
	
