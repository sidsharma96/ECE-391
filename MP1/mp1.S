
# mp1.S - missile-command version
# Good luck, and have fun!

.data
	
    # Constants for accessing the fields of a struct missile, 
    # struct missile is defined in rtc.h and mp1.h

	NEXT 	  = 0
	X	  = 4
	Y	  = 8
	VX	  = 12
	VY	  = 16
	DEST_X	  = 20
	DEST_Y	  = 24
	EXPLODED  = 28
	C	  = 32
	PLUS = 43

    # Character to draw for an explosion - '@'

	EXPLOSION = 64
	OFFSET = 3876
	PLUS = 43
	SPACE = 32
	SHIFT = 16
	ONESIXTY = 160
	TWO = 2
	STRUCT = 33
	UPPERX = 79
	UPPERY = 24
	INIT_CROSSX =  40
	INIT_CROSSY = 12


# Data shared between this file and rtc.c helper functions

# This '.globl' directive makes these symbols visible externally
.globl mp1_missile_list, base_alive, mp1_score

mp1_missile_list:	.long 0x0	# Head of list pointer
base_alive:		.long 0x0	# Array of 3 bytes, plus a padding byte
mp1_score:		.long 0x0	# Player's current score


# Data private to this  file

base_pic:     	    .string "/^^^\\"	# Picture of a live base
dead_base_pic: 	    .string "xxxxx"	# Picture of a dead base
crosshairs_x:		.long 0x0	# X-position of the crosshairs
crosshairs_y:		.long 0x0	# Y-position of the crosshairs

.text

# void mp1_poke(void);
# You can use this function to write to video memory.
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX

mp1_poke:
		movl    vmem_base_addr(,1),%edx
		movb    %cl,(%edx,%eax,1)
		ret

#   ----------------- Exported functions ---------------------

# void mp1_rtc_tasklet(unsigned long garbage);
# Performs three tasks:
#	(1) updates the list of missiles (implement this in update_missiles,
#           below, and call it from here).
#	(2) Redraw the bases - they may have been overwritten with missiles
#	(3) Redraw the crosshairs - it may have been overwritten with missiles
# Inputs   : none
# Outputs  : none
# Registers: Standard C calling convention

.globl mp1_rtc_tasklet
mp1_rtc_tasklet:

	pushl %ebp
	movl %esp, %ebp

	pushl %ebx
	pushl %esi
	pushl %edi

	
							#calling the 3 functions needed for us to implement
	call update_missiles

	call redraw_cities

	call redraw_crosshairs

	

	popl %edi
	popl %esi
	popl %ebx

	leave
	ret
	

# void redraw_cities() 
# Draws dead or alive cities
# Inputs   : none
# Outputs  : none	

redraw_cities:
		pushl %ebp
		movl %esp, %ebp
		push %ebx
		pushl %esi
		pushl %edi
	

		leal base_alive, %ecx 	#Storing base_alive address
		movl $OFFSET, %edx  #Shifting offset (18*2+(24*160) = 3876)
		movl $3, %edi 	#setting counter to 3 for 3 cities

	
	redraw_loop:			#loop for all
		cmpb $0, (%ecx) #checking first city
		je deadcity
		jmp alivecity


	deadcity: 				#if dead, draw dead characters
		movl $dead_base_pic, %esi
		movl %edx, %eax
		movl $5, %ebx

	deadloop:
		movb (%esi), %cl
		pushl %edx
		pushl %ecx
		call mp1_poke
		popl %ecx
		popl %edx

		addl $2, %eax 	#moving  to next space on screen
		addl $1, %esi 	#moving to next character in dead_base_pic

		subl $1, %ebx		#checking to go to next city
		cmpl $0, %ebx
		je cityloop
		jmp deadloop

	alivecity:			#if alive, draw alive characters
		movl $base_pic, %esi
		movl %edx, %eax
		movl $5, %ebx

	aliveloop:      
		movb (%esi), %cl
		pushl %edx
		pushl %ecx
		call mp1_poke
		popl %ecx
		popl %edx

		addl $2, %eax 	#moving to next space on screen
		addl $1, %esi 	#moving to  next character in base_pic

		subl $1, %ebx
		cmpl $0, %ebx
		je cityloop        #checking to go to next city
		jmp aliveloop

	cityloop: 				#jumping to next city
		addl $32, %edx 		#moving to next space on screen to draw next city
		addb $1, %cl 		#moving to next city in base alive array

		subl $1, %edi 		#city counter
		cmpl $0, %edi
		jg redraw_loop
		jmp redraw_finish
	
	redraw_finish:
					#tearing down stack
		popl %edi
		popl %esi
		popl %ebx
		leave
		ret

# void redraw_crosshairs() 
# redrawing crosshairs at location
# Inputs   : none
# Outputs  : none

redraw_crosshairs:
	
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	
	movl crosshairs_y, %edi 		#(160*y + 2*x)
	imull $ONESIXTY, %edi
	movl crosshairs_x, %ebx
	imull $TWO, %ebx
	addl %ebx, %edi
	movl %edi, %eax
	movb $PLUS, %cl 			#drawing crosshair character
	
	pushl %edx
	pushl %eax
	pushl %ecx
	call mp1_poke
	popl %ecx
	popl %eax
	popl %edx
	
	popl %edi
	popl %esi
	popl %ebx
	
	leave
	ret
		
# int mp1_ioctl(unsigned long arg, unsigned int cmd)
# The dispatch function for the MP1 ioctls - should use the cmd argument
# and a jumptable to execute one of the specific ioctls implemented below.
# Inputs   : unsigned long arg - parameter to the mp1_ioctl_....
#          : unsigned int cmd  - specifies which mp1_ioctl_... function 
#          :                     to execute
# Outputs  : Returns an integer - depends on which ioctl() function is called
# Registers: Standard C calling convention


.globl mp1_ioctl
mp1_ioctl:

		
		movl 8(%ebp), %ecx 		 		#making dispatcher function

		cmpl $0, %ecx
		jb minus
		cmpl $4, %ecx
		ja minus

		jmp *jumptable(,%ecx,4)

	minus:
		movl $-1, %eax
		
		ret

jumptable:
		.long mp1_ioctl_startgame, mp1_ioctl_addmissile, mp1_ioctl_movexhairs, mp1_ioctl_getstatus, mp1_ioctl_endgame
#

# ----------------- Functions private to this file -------------------



# void update_missiles() 
# checks each of the tasks appointed to us
# Inputs   : none
# Outputs  : none


update_missiles:
		pushl %ebp
		movl %esp, %ebp
		pushl %ebx
		pushl %esi
		pushl %edi
		movl mp1_missile_list, %edx 		#holds list
		movl $0, %edi #holds previous pointer

		
		
	getnxt:	
		cmpl $0, %edx   #checking whether first case is present
		je update_finish
		 
		cmpl $0, EXPLODED(%edx)		#checking whether exploded
		jne current_explode

		movl X(%edx), %ebx #X - position
		sarl $SHIFT, %ebx
		movl Y(%edx), %esi #Y-position
		sarl $SHIFT, %esi

		imull $ONESIXTY, %esi
		imull $TWO, %ebx
		addl %esi, %ebx
		movl %ebx, %eax
		movb $SPACE, %cl 		#erasing missile

		pushl %edx
		call mp1_poke
		popl %edx

		movl Y(%edx), %esi  	#updating missile Y position  
		addl VY(%edx), %esi
		movl X(%edx), %ebx 		#updating missile X position
		addl VX(%edx), %ebx

		movl %ebx, X(%edx)
		movl %esi, Y(%edx)
		sarl $SHIFT, %esi  		#shifitng to see text-video location
		sarl $SHIFT, %ebx 		#shifting to see text-video location
			
		cmpl $UPPERX, %ebx #checking X pos whether out of bounds
		ja outofsc
		cmpl $0, %ebx
		jb outofsc
			
		cmpl $UPPERY, %esi #checking Y pos whether out of bounds
		ja outofsc
		cmpl $0, %esi
		jb outofsc
		
		cmpl DEST_Y(%edx), %esi #checking if destination reached
    	jne changePos
		
	checkx: 
		cmpl DEST_X(%edx), %ebx 
		je current_explode
		jmp changePos 		# drawing new position
			
	outofsc:


		pushl %eax
		pushl %edx
		pushl %ecx

		call mp1_missile_remove # erases missile and frees memory

		popl %ecx
		popl %edx
		popl %eax

		movl %edx, %edi #prev node = curr node
		cmpl $0, %edx
		je update_finish # 

		movl NEXT(%edx), %edx # curr=curr->next
		cmpl $0, %edx
		je update_finish
		jmp getnxt


	current_explode:
		movl $0, VX(%edx)  #stopping missile
		movl $0, VY(%edx)		
		
		pushl %edx          #calling missile explode
		call missile_explode
		popl %edx

		cmpl $0, %eax   
		je not

		pushl %eax
		pushl %ecx
		pushl %edx

		call mp1_notify_user

		popl %edx
		popl %ecx
		popl %eax

	not:
	
		
		subl $1, EXPLODED(%edx) #decrementing exploded field
		cmpl $0, EXPLODED(%edx)
		jne draw
		
		movl X(%edx), %ebx #X- postion
		sarl $SHIFT, %ebx
		movl Y(%edx), %esi #Y-position
		sarl $SHIFT, %esi

		imull $ONESIXTY, %esi #erasing missile
		imull $TWO, %ebx
		addl %esi, %ebx
		movl %ebx, %eax
		movb $SPACE, %cl

		pushl %edx 
		call mp1_poke
		popl %edx

		jmp outofsc
			
	draw:

	
		movl X(%edx), %ebx #X - position
		sarl $SHIFT, %ebx
		movl Y(%edx), %esi #Y-position
		sarl $SHIFT, %esi

		imull $ONESIXTY, %esi #Drawing explosion character
		imull $TWO, %ebx
		addl %esi, %ebx
		movl %ebx, %eax
		movb $EXPLOSION, %cl

		pushl %edx
		call mp1_poke
		popl %edx

		movl %edx, %edi 	#prev node = curr node
		cmpl $0, %edx
		je update_finish

		movl NEXT(%edx), %edx  # curr=curr->next
		cmpl $0, %edx
		je update_finish
		jmp getnxt
			
		

		
	changePos:
		movl X(%edx), %ebx #Storing updated y and x positions of missile in registers
		movl Y(%edx), %esi

		sarl $SHIFT, %esi #Shifting to text-video location
		sarl $SHIFT, %ebx

		
		imull $ONESIXTY, %esi #Printing missile in new position
		imull $TWO, %ebx
		addl %esi, %ebx
		movl %ebx, %eax
		movb C(%edx), %cl

		pushl %edx
		call mp1_poke
		popl %edx
			
		movl %edx, %edi #prev node = curr node
		cmpl $0, %edx
		je update_finish

		movl NEXT(%edx), %edx # curr=curr->next
		cmpl $0, %edx
		je update_finish
		jmp getnxt			
			
	update_finish:
	 				#tearing down stack
		popl %edi
		popl %esi
		popl %ebx

		leave
		ret

#int mp1 ioctl startgame (unsigned long ignore);
# initializing the variables
# Inputs   : unsigned long ignore
# Outputs  : -1 if fail


mp1_ioctl_startgame:
		pushl %ebp
		movl %esp, %ebp
		pushl %ebx
		pushl %esi
		pushl %edi
		movl $INIT_CROSSX, crosshairs_x  #initializing crosshairs
		movl $INIT_CROSSY, crosshairs_y
		movl $0x00ffffff, base_alive #initializing base alive
		movl $0x0, mp1_missile_list #initializing mp1_missile_list
		movl $0x0, mp1_score #initializing mp1_score

		popl %edi
		popl %esi
		popl %ebx

		leave
		ret

#int mp1 ioctl addmissile (struct missile* user missile;
# adding missile to linked list
# Inputs   : struct missile pointer
# Outputs  : -1 if fail


mp1_ioctl_addmissile:

		pushl %ebp
		movl %esp, %ebp
		pushl %ebx
		pushl %esi
		pushl %edi

		

		movl $STRUCT, %ecx #Pushing size of structure 
		pushl %ecx
		call mp1_malloc #Making new dynamically allocated memory
		popl %ecx
		

		movl 8(%ebp), %edi #storing argument

		pushl %ecx 	#size argument
		pushl %edi  #pointer from argument
		pushl %eax  #pointer to argument

		call mp1_copy_from_user

		cmpl $0, %eax #checking if it fails or succeeds
		jne add_fail


		popl %eax
		popl %edi
		popl %ecx

		movl mp1_missile_list, %edx  #new_missile -> next = head
		movl %edx, NEXT(%eax)
		movl %eax, mp1_missile_list

		movl $0, %eax

		jmp add_finish

	add_fail:   
		popl %eax
		popl %edi
		popl %ecx

		pushl %edx
		pushl %ecx

		pushl %eax 			#freeing memory allocated
		call mp1_free
		popl %eax

		popl %ecx
		popl %edx
		

		movl $-1, %eax  	#returning -1 if failed
		jmp add_finish

	add_finish:
						#tearing down stack
	
		popl %edi
		popl %esi
		popl %ebx

		leave

		ret

#int mp1 ioctl movexhairs (unsigned long xhair delta packed);
# moving crosshairs on screen
# Inputs   : 32 bit integer, carrying crosshairs locations
# Outputs  : 0


mp1_ioctl_movexhairs:
		pushl %ebp
		movl %esp, %ebp
		pushl %ebx
		pushl %esi
		pushl %edi
		


		movl crosshairs_x, %eax     #getting crosshairs x position
		movl crosshairs_y, %ebx 		


		imull $ONESIXTY, %ebx 		#erasing character of crosshair
		imull $TWO, %eax
		addl %ebx, %eax
		movb $SPACE, %cl

		pushl %edx
		call mp1_poke
		popl %edx
		
		movl 8(%ebp), %ecx   		# storing crosshairs_x position in low 16 bits
		movw %cx, %dx
		movl crosshairs_x, %eax
		addw %dx, %ax

		movl %eax, crosshairs_x 	#updating crosshairs_X
		 
		
		
		movl 8(%ebp), %ecx 			# storing crosshairs_y position in high 16 bits of ecx
		sarl $SHIFT, %ecx 			
		movl %ecx, %edi
		addl %edi, crosshairs_y		#updating crosshairs_y

		
	
	checkxup:
		cmpl $UPPERX, crosshairs_x 		#checking whether crosshairs_x in screen or not
		jg xupper
		cmpl $0, crosshairs_x
		jge checkyup
		movl $0, crosshairs_x 		#setting crosshairs_x to 0
	
	checkyup:
		cmpl $UPPERY, crosshairs_y 	#checking whether crosshairs_y in screen or not
		jg yupper
		cmpl $0, crosshairs_y
		jge cross_draw
		movl $0, crosshairs_y	#setting crosshairs_y to 0
		jmp cross_draw
		
	xupper:
		movl $UPPERX, crosshairs_x  #setting crosshairs_x to 79
		jmp checkyup
	
	yupper:
		movl $UPPERY, crosshairs_y  #setting crosshairs_y to 24
		jmp cross_draw
	
	
	cross_draw:

		movl crosshairs_x, %eax  #drawing both the crosshairs
		movl crosshairs_y, %ebx

		imull $ONESIXTY, %ebx
		imull $TWO, %eax
		addl %ebx, %eax
		movb $PLUS, %cl
		
		pushl %edx
		call mp1_poke
		popl %edx
		
		jmp cross_finish
		
	cross_finish:		#tearing down stack

		xorl %eax, %eax
		
		popl %edi
		popl %esi
		popl %ebx
		
		leave
		ret



#int mp1 ioctl getstatus (unsigned long* user status)
# Pushing the score and status of cities into a 32 bit integer
# Inputs   : 32 bit integer pointer
# Outputs  : 0 if succeeds and -1 if fails


mp1_ioctl_getstatus:
		pushl %ebp
		movl %esp, %ebp
		pushl %ebx
		pushl %esi
		pushl %edi
		
		movl 8(%ebp), %ecx #storing argument in ecx

		
		leal base_alive, %eax #storing base array in eax
		
		
	city1:
		movb 0(%eax), %cl #storing first city status in ecx
		
	city2:
		shlb $1, %cl #storing next city status in ecx
		movb 1(%eax), %cl
			
	city3:
		shlb $1, %cl #storing next city status in ecx
		movb 2(%eax), %cl
	
	status_next:

		shlb $SHIFT, %cl #shifting ecx by 16 bits

		movw mp1_score, %cx #storing score in lower 16 bits of ecx
		

	
		pushl %ecx 		#pushing ecx on stack
		movl %esp, %esi
		movl 8(%ebp), %edx

		movl $4, %edi #size of long

		pushl %edi #long size
		pushl %esi #pointer to ecx
		pushl %edx #pointer to where ecx needs to be stored
		
		call mp1_copy_to_user
		
		popl %edx #popping arguments
		popl %esi
		popl %edi

		popl %ecx
		
		cmpl $0, %eax #if success, 0 returned
		jne status_fail
		jmp status_finish

	status_fail:

		movl $-1, %eax #if status fails, -1 returned
		jmp status_finish

	status_finish:
		
					#tearing down stack
		popl %edi
		popl %esi
		popl %ebx

		leave
		ret

#int mp1 ioctl endgame (unsigned long ignore)
# ends game and frees memory in linked list 
# Inputs   : ignored
# Outputs  : 0 if success

mp1_ioctl_endgame:
		pushl %ebp
		movl %esp, %ebp
		pushl %ebx
		pushl %esi
		pushl %edi

	end_loop:
		cmpl $0, mp1_missile_list    #checking if head is 0
		je endgame_finish    #if it is, then end

		movl mp1_missile_list, %edx   # head = head->next
		movl NEXT(%edx), %edi
		movl %edi, mp1_missile_list
		
		pushl %edx
		call mp1_free # free head
		popl %edx
		jmp end_loop #loop to free all missiles

	endgame_finish:
					#tearing down stack
		popl %edi
		popl %esi
		popl %ebx

		leave
		ret

#helper function for update_missiles

mp1_missile_remove:
	pushl %ebp
	movl %esp, %ebp 
	pushl %ebx
	pushl %esi
	pushl %edi

	cmpl $0, mp1_missile_list #checking if start missile should be removed
	je missile_remove_finish

	cmpl mp1_missile_list, %edx #this means that current missile is head of list
	je first_missile
	
	movl NEXT(%edx), %ebx #ebx = curr->next
	movl %ebx, NEXT(%edi) #prev->next = curr->next
	movl $0, NEXT(%edx)

	pushl %edx
	call mp1_free  #freeing memory 
	popl %edx

	jmp missile_remove_finish

first_missile:
	movl NEXT(%edx), %eax  #eax = head->next
	movl %eax, mp1_missile_list #head = head->next

	pushl %eax
	pushl %ecx
	pushl %edx

	call mp1_free  #freeing memory associated with missile
	
	popl %edx
	popl %ecx
	popl %eax

	movl mp1_missile_list, %edx 		#storing new head in edx

missile_remove_finish:
	popl %edi
	popl %esi
	popl %ebx

	
	leave
	ret

