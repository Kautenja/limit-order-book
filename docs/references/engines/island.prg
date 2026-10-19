SET TALK OFF
SET CURSOR OFF
SET SAFETY OFF
SET MEMOWIDTH TO 80
CLEAR ALL
CLOSE ALL
CLEAR
CLEAR MACRO		&& Get rid of F keys typing commands
SET HELP OFF	&& Needed to clear out F1 key too

=rand(-1)

#define COREDEST "FFFFFFFF0673"

gstoredflag = .F.		&& Are there any messages buffered that could be flushed?

#define VERSION "2.40"

malertfile = "M:\logs\island2.err"
mholdofffile = "M:\island2\holdoff.txt"

blanknbuffer="R"+replicate( " " , 200 )                 && get BRIGADEN message here

#define CORELINELEN 124
#define COREMESSAGELEN 122

#define READPORT "067F"

load int99

define window title    from 0,0   to  0,scols()-1  COLOR "W/B" NONE
define window output   from 1,0   to 10,scols()-1 NONE COLOR "W+/N"
define window ticker   from 12,0   to srows()-1,scols()-1 NONE
define window status   from 11,0   to 11,scols()-1 COLOR "W+/R" NONE

do title 
do initstatus

activate window title
activate window status

activate window output
activate window ticker


s = fopen( "ISLAND2.TXT" )

if s <= 0 
	wait window "COULD NOT FIND ISLAND2.TXT IN CURRENT DIR. RUN RESET."
	quit
endif

x = fgets( s )

if "ISLAND2" # x
	wait window "ACCOUNT IN ISLAND2.TXT DOES NOT MATCH ACCOUNT ENV VAR. RUN RESET."
	=fclose( s )
	quit
endif

tcount = 0 		&& For counting random stuff

tseq = val( fgets( s ) )				&& Next order number
tmatch = val( fgets( s ) )				&& Next match number
tnextdead = val( fgets( s ) )			&& Next dead order (zero=none)
=fclose( s )

select select(1)
use events
set order to WHEN

tnow = 0

ttoolate = 0 			&& Count the too late to cancel events

gaccept    = .F.		&& Let any orders in? Set by SOD, cleared by EOD
gactaccept = .F.		&& Let ACT orders in? Set by SOD, cleared by EOA

geom	   = .F.		&& Have we gotten EOM yet?

gprinting  = .F. 		&& Print incoming messages?

go top

if eof("EVENTS")

	tnext = 99999.999
	
else

	tnext = WHEN			&& Time of next event
	
endif

select 0
use sources
set order to SOURCE

select select(1)
use island

gsx = alltrim( str( date() - {2/6/1996} ) )

gsession = left( "0000000000" , 9-len( gsx ) ) + gsx + "F"

s="N"+gsession+"5050"			&& Open the islandfile for writing Mold on port 5050
call int99 with s

if s # "n" && Valid open file?

	do alert with "InitMold filed with reason >"+s
	
	do case
	
		case s = "i"
		
			sm ="Error opening files in deney-write mode"
			
		case s = "j"
		
			sm = "a file had a partial line fragment"
			
		case s = "k"
		
			sm = "not all trailing files are empty"
			
		case s = "l"
		
			sm = "all files full"
		
		otherwise
		
			sm = "Unknown error"

	endcase
	
	do alert with sm
	
	wait window "Error on ISLAND.OUT or ISLAND2.OUT OPEN:"+s
	
	quit
		
endif

select island

tecount  = 0					 	&& Assume worst case, just to make sure ecount is high enough
txcount  = 0						&& Cancels

x = "L"+READPORT+" "		&& Read Island2 requests
call int99 with x
if x # "l"
	do alert with "Could not open Command listen!"
endif


gstatdelay = 0.25
do showstatus
gnextstats = seconds() + gstatdelay

gmolddelay = 1							&& Time between mold heartbeats
gmoldheart = seconds() + gmolddelay		&& Time to send a MOLD heartbeat by doing a FLUSH

gmessages = 0			&& Number of messages written to the stream
gpackets = 0			&& Number of packets writtento the stream

mdone = .F.

do while !mdone

	tnow = seconds()
	
	do while tnow >= tnext
		
		if !EVENTS.PROCESSED
		
			do swrite with "G" , EVENTS.CODE
			
			select EVENTS		
			replace PROCESSED with .T.
			select ISLAND
			
		else
		
			? time()+" Event bypassed:"+ EVENTS.CODE
		
		endif
		
		do seteventflags with EVENTS.CODE
		
		skip 1 in EVENTS
		
		if eof("EVENTS")
		
			tnext = 99999.999
			
		else
		
			tnext = EVENTS.WHEN
		
		endif
		
	enddo
		
	nbuffer = blanknbuffer
	
	call int99 with nbuffer
	
	if nbuffer = "r"

		tlenstr = substr( nbuffer , 2 , 5 )
								
		tlen = val( tlenstr )
					
		tmessage = substr( nbuffer , 43 , tlen )
				
		if gprinting
			? time()+" >"+tmessage+"<"
		endif
								
		if tmessage = "P"	&& PING REQUEST
				
			tsource = substr( nbuffer ,11 , 8)
					
			do pingreply with tsource , tmessage
					
		else
		
			do processtcpip with tmessage
				
		endif

	else && No pending commands...
				
		if gstoredflag 
		
			do flush
		
			gstoredflag = .F.
			
			gmoldheart = seconds() + gmolddelay
			
			gpackets = gpackets + 1
						
		else
		
			if gmoldheart < seconds()
			
				do flush
				
				gmoldheart = seconds() + gmolddelay
				
			endif
			
		endif	
	
	endif
	
	if gnextstats <= seconds()
	
		do showstatus
		gnextstats = seconds() + gstatdelay	
	
	endif
	
	lastkey=inkey()
	
	if lastkey # 0
	
		activate window output
	
		do case
			case lastkey = asc("~")
				? time()+" Quitting..."
				mdone = .T.
			
			case lastkey = asc("@")
				? time()+" Copying to island2.dbf"
				set order to 
				copy to m:\island2\island2
				?? "Done"
			
			case lastkey = asc("#")
				? time()+" Suspending..."
				suspend
			
			case lastkey = asc("!")
			
				? time()+" ACCEPT="+iif(gaccept,"T","F")+" ACT="+ iif(gactaccept,"T","F") + " EOM="+iif(geom,"T","F")+" NEXT="+str(tnext,9,3)+ " TOO-LATE="+str(ttoolate,6,0)+" COUNT="+str(tcount,9,0)

				s = "T"+space(60)
				call int99 with s
			
				if s = "t"
				
					tbuffered = val( substr(s,26,5) )
						
					? "MOLD INFO: SESSION="+substr(s,2,10)+" SEQ="+substr(s,12,10)+" SOCKET="+substr(s,22,4)+" BUFFERED="+str(tbuffered,5,0)
				
				else
			
					? "Could not get mold info!"
					
					tbuffered = 0
				
				endif

				
				if gpackets > 0 
				
					? "MESSAGES:"+str(gmessages-tbuffered,12,0)+" PACKETS:"+str(gpackets,12,0)+" ("+str( (gmessages-tbuffered)/ gpackets , 5 , 3 )+")"

					gmessages = tbuffered
					gpackets  =0
					
				endif
							
			case lastkey = asc("%")
				? time()+" Shutdown attempt..."
				do shutdown
				
			case lastkey = asc("(")
				? time()+" toggled gaccept"
				gaccept = !gaccept
			
			case lastkey = asc("?")
				? "@-Copy to island2.DBF ~-Quit !-Status #-Suspend %-ShutDown $-Print incoming"
				
			case lastkey = asc("$")
			
				if gprinting
				
					? time()+" Printing off"
					gprinting = .F.
				else					
				
					? time()+" Printing on"
					gprinting = .T.
				
				endif
				
					
		endcase
		
		activate window ticker
			
	endif
		
enddo  && Main Loop

activate window output

&& Final flush to mak sure everything is sent

do flush

&& Close file

s = "C"
call int99 with s

x = "H"+READPORT
call int99 with x
if x # "h"
	do alert with "Could not close PingPort!"
endif

use			&& Unuse Island

&& Create fresh waypoint file

f = fcreate("ISLAND2.TXT")

if f<=0 
	? "Could not create ISLAND.TXT!"
	suspend
endif

=fputs( f , "ISLAND2" )					&& ROLE

=fputs( f , str( tseq , 9 , 0   )  )	&& Order number
=fputs( f , str( tmatch , 9 , 0 )  )	&& Match number
=fputs( f , str( tnextdead , 9 , 0 ) ) 	&& Next dead order pointer
=fclose(f)
	
quit


procedure title
	activate window title SAME
	clear
	@ 0,0 SAY " Island2  ú Version "+VERSION+" (c)1996 Joshua Levine ú Press [?] for help"
	activate window ticker SAME
return 

proc werror
	param wcode
	
	do case
	
		case wcode = "w"
		
			return "Error on file write"
			
		case wcode = "s"
		
			return "Error on nework send"
			
		case wcode = "c"
		
			return "Invalid message length"
			
		case wcode = "d"
		
			return "all files full"
						
	endcase
	
return "Unknown error"




**** write actualy writes a string the the file, steam, and screen

proc write
	parameter wstring
		
	wl = len( wstring )
	
	ws="W"+chr(wl)+wstring

 	call int99 with ws
 	
 	gmessages = gmessages + 1
 	
	if ws = 'b'
	
		gstoredflag = .T.
		
	else
	
		if ws = "f"
		
			gpackets = gpackets + 1
			
			gstoredflag = .F.
						
		else
		
			activate window output		
	
			? time() +" !!!! ERROR ON WRITE!!!!!!"

			? ws
			
			do alert with "Erorr on write:"+werror(ws)
			
			activate window ticker
				
			suspend
			
		endif
		
	endif
	
return



*** Write a message

**** ACTIONS:
**** A - Accept 	the order was accepted into Island
**** B - Booked		this order hit the book
**** E - Execute	the order was executed for this many shares at this price
**** X - Cancel		this many shares were canceled
**** C - Break		this order was executed, now broken
**** G - Control    stock is the control type
**** R - Report     trade done away but will report/clear through Island

**** ON Cancel CONTRA = Reason for cancel


***** MINDICATE = on accept always "D" (legacy)
*****  			on execute "A"= added liquidity, "R"=Removed liquidity
*****           on report "Y" trade report, "N" don't report, "S" step-out

****** mmatch = on accept or book is MINIMUM shares, in execution is match number
******			not defined on cancels, but 0 for now

proc mwrite

	parameter maction,mseq,mshares,mprice,mcontra,mindicate,mmatch,mlocate
	
	mwhen = str( tnow , 9 , 3)
	
	w = left( mwhen , 5 ) + "," +maction+","+str(mseq,9,0)+","+PORT+","+USER+","+TOKEN+","+BUY_SELL+","+str(mshares,9,0)+","+str(mmatch,9,0)+","+STOCK+","+str(mprice,11,4)+"," +str(mlocate,8,0)+ ",    0,"+SHORT+","+MMID+","+PA+","+mcontra+","+mindicate+","+DISPLAY+","+right(mwhen,3)+","+CLEARING+",D"

	do write with w
	 
return

****** Swrite writes a status message that doesnot concern an order, Like G-Good morning

proc swrite

	param maction , mcode
	
	mwhen = str( tnow , 9 , 3)
			
	xcode = left( padr( mcode , 3) , 3 )
	
	w = left( mwhen , 5 ) +","+maction+","+"        0"+",      ,    ,          , ,        0,        0,*"+xcode+"  ,     0.0000,00000000,    0, ,    , ,    , , ,"+right( mwhen , 3 )+", , "

	do write with w
	 	
return

****** Enter order adds the audit, tries for a match, and if it don't work, books it.
****** Enter order assumes ostock and obuy_sell are the right length.
****** Also assumes that buy_sell has aready been checked to be B or S. 
****** Also assumes that the token is no already used

proc enter2order

	parameter oport,ouser,otoken,obuy_sell,oshares,ostock,oprice,otif,oshort,ommid,opa,odisplay,omin,oclearing,oflags
	
	
	if odisplay = "P"
	
		obasefirm = ouser
		
	else 
	
		obasefirm = "####"
		
	endif
	
	&& Get the working record
	
	if tnextdead > 0 
	
		tlocate = tnextdead
		goto tnextdead
		tnextdead = SEQ
		
	else
	
		insert into ISLAND (LEAVES) values (0)		&& Keep it out of the indexes for now
		tlocate = recno()
		
	endif
					
	tseq = tseq + 1
										 
	awhen = str( tnow , 9 , 3)
	
	oleaves = oshares
	
	ofilled = 0
		
	aflag = .f.		&& have we written the "A" message yet?
		
	if obuy_sell = "B"		&& Buy order...
						
		set order to SSEEK 
		
		do case
										
			case omin > 1		&& Minimum quantity specified?
			
				*** if the min is bigger than the size, set it to the size
		
				if omin > oshares
				
					mmin = oshares
					
				else
	
					mmin = omin
					
				endif
				
				*** First prescan to see if we have enough size to fill it
							
				if seek( ostock )
									
					scan while mmin > 0 .and. ostock = STOCK .and. BUY_SELL = "S" .and. oprice >= PRICE
				
						mmin = mmin - LEAVES
				
					endscan
				
				endif
				
				if mmin <= 0 
				
					*** There were at least enough to fill the minimum quantity
				
					mmax = oshares
					
				else
				
					*** Not enough to fill the min, so fill none
				
					mmax = 0
					
				endif
				
			case odisplay = "R" .or. odisplay = "P"	&& Is this a round-lot only order?
			
				mmax = 0	

				*** Prescan to find the number of a shares available
							
				if seek( ostock )
									
					scan while mmax <= oshares .and. ostock = STOCK .and. BUY_SELL = "S" .and. oprice >= PRICE .and. obasefirm # MMID
				
						mmax = mmax + LEAVES
				
					endscan
				
				endif
				
				if mmax >= oshares
				
					*** Max out with the number of shares specified
									
					mmax = oshares
					
				endif
				
				*** Round down to nearest round lot wihtout using floating point
				
				mmax = val( substr( str( mmax , 6, 0 ) , 1 ,4 ) + "00" )
					
				
			otherwise
				
				mmax = oshares
				
		endcase
			
		do while mmax > 0 .and. seek( ostock  ) .and. oprice >= PRICE
		
			if !aflag

				w = left( awhen , 5 ) +",A," +str(tseq,9,0) +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(oshares,6,0) +",   " +str(omin,6,0) +"," +ostock +"," +str(oprice,11,4) +"," +str(tlocate,8,0) +"," +str(otif,5,0) +"," +oshort +"," +ommid +"," +opa+","+oflags+", ," +odisplay +"," +right(awhen,3) +"," +oclearing+",D" 

				do write with w
				
				aflag = .T.
								
			endif
			
			tecount = tecount + 1
			
			eshares = min( LEAVES , mmax )
				
			oleaves = oleaves - eshares
			
			mmax = mmax - eshares
				
			ofilled = ofilled + eshares
																
			tmatch = tmatch + 1
			
			w = left( awhen , 5 ) +",E," +str( SEQ ,9,0) +"," +PORT +"," +USER +"," +TOKEN +"," +BUY_SELL +",   " +str(eshares,6,0) +"," +str(tmatch,9,0) +"," +ostock +"," +str(PRICE,11,4) +"," +str( recno() ,8,0)+ ",    0," +SHORT +"," +MMID +"," +PA+"," +ommid+",A," +DISPLAY +"," +right(awhen,3) +"," +CLEARING+",D"

			do write with w

			w = left( awhen , 5 ) +",E," +str(tseq,9,0)    +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(eshares,6,0) +"," +str(tmatch,9,0) +"," +ostock +"," +str(PRICE,11,4) +"," +str(tlocate,8,0)+",    0," +oshort +"," +ommid +"," +opa+","+MMID+",R," +odisplay +"," +right(awhen,3) +"," +oclearing+",D"

			do write with w


			dleaves = LEAVES - eshares
			
			if dleaves <= 0 		&& Dead record?
			
				replace LEAVES with 0 , SEQ with tnextdead
				tnextdead 	= recno()
				
			else

				replace LEAVES with dleaves , FILLED with FILLED + eshares 
				
			endif
									
		enddo
									
	else && sell order..
			
		set order to BSEEK
			
		do case
										
			case omin > 1		&& Minimum quantity specified?
			
				*** if the min is bigger than the size, set it to the size
		
				if omin > oshares
				
					mmin = oshares
					
				else
	
					mmin = omin
					
				endif
				
				*** First prescan to see if we have enough size to fill it
							
				if seek( ostock )
									
					scan while mmin > 0 .and. ostock = STOCK .and. BUY_SELL = "B" .and. oprice <= PRICE
				
						mmin = mmin - LEAVES
				
					endscan
				
				endif
				
				if mmin <= 0 
				
					mmax = oshares
					
				else 
				
					mmax = 0
					
				endif
				
			case odisplay = "R" .or. odisplay = "P" && Is this a round-lot only order?
			
				mmax = 0	

				*** Prescan to find the number of a shares available
							
				if seek( ostock )
									
					scan while mmax <= oshares .and. ostock = STOCK .and. BUY_SELL = "B" .and. oprice <= PRICE .and. obasefirm # MMID
				
						mmax = mmax + LEAVES
				
					endscan
				
				endif
				
				if mmax >= oshares
				
					mmax = oshares
					
				endif
				
				*** Round down to nearest round lot wihtout using floating point
				
				mmax = val( substr( str( mmax , 6, 0 ) , 1 ,4 ) + "00" )
					
				
			otherwise
				
				mmax = oshares
				
		endcase

		do while mmax > 0 .and. seek( ostock  ) .and. oprice <= PRICE 
			
			if !aflag

				w = left( awhen , 5 ) +",A," +str(tseq,9,0) +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(oshares,6,0) +",   " +str(omin,6,0) +"," +ostock +"," +str(oprice,11,4) +"," +str(tlocate,8,0) +","+str(otif,5,0) +"," +oshort +"," +ommid +"," +opa+","+oflags+", ," +odisplay +"," +right(awhen,3) +"," +oclearing+",D" 

				do write with w
				
				aflag = .T.
									
			endif
			
			tecount = tecount + 1
			
			eshares = min( LEAVES , mmax )
				
			oleaves = oleaves - eshares
			
			mmax = mmax - eshares
				
			ofilled = ofilled + eshares
													
			tmatch = tmatch + 1
								
			w = left( awhen , 5 ) +",E," +str( SEQ ,9,0) +"," +PORT +"," +USER +"," +TOKEN +"," +BUY_SELL +",   " +str(eshares,6,0) +"," +str(tmatch,9,0) +"," +ostock +"," +str(PRICE,11,4) +"," +str( recno() ,8,0) +",    0," +SHORT +"," +MMID +"," +PA+"," +ommid+",A," +DISPLAY +"," +right(awhen,3) +"," +CLEARING+",D"

			do write with w

			w = left( awhen , 5 ) +",E," +str(tseq,9,0)    +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(eshares,6,0) +"," +str(tmatch,9,0) +"," +ostock +"," +str(PRICE,11,4) +"," +str(tlocate,8,0) +",    0," +oshort +"," +ommid +"," +opa+","+MMID+",R," +odisplay +"," +right(awhen,3) +"," +oclearing+",D"

			do write with w

			dleaves = LEAVES - eshares
			
			if dleaves <= 0 		&& Dead record?
			
				replace LEAVES with 0 , SEQ with tnextdead
				tnextdead 	= recno()
				
			else

				replace LEAVES with dleaves , FILLED with FILLED + eshares 
				
			endif
											
		enddo
															
	endif
		
	if oleaves > 0			&& any non-executed shares left?
	
		if otif = 0				&& Fill or kill, so cancel leaves

			if !aflag		&& did we not make an A message yet?
			
				w = left( awhen , 5 ) +",A," +str(tseq,9,0) +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(oshares,6,0) +",   " +str(omin,6,0) +"," +ostock +"," +str(oprice,11,4) +"," +str(tlocate,8,0)+ "," +str(otif,5,0) +"," +oshort +"," +ommid +"," +opa+","+oflags+", ," +odisplay +"," +right(awhen,3) +"," +oclearing+",D"
				do write with w		
				
			endif
		
			w = left( awhen , 5 ) +",X," +str(tseq,9,0)    +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(oleaves,6,0) +",        0," +ostock +"," +str(oprice,11,4) +"," +str(tlocate,8,0)+ ",    0," +oshort +"," +ommid +"," +opa+",#IOC, ," +odisplay +"," +right(awhen,3) +"," +oclearing+",D"
			do write with w		
			
			oleaves = 0
									
		else
		
			if !aflag		&& did we not make an A message yet?
						
				w = left( awhen , 5 ) +",A," +str(tseq,9,0) +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(oshares,6,0) +",   " +str(omin,6,0) +"," +ostock +"," +str(oprice,11,4) +"," +str(tlocate,8,0) +"," +str(otif,5,0) +"," +oshort +"," +ommid +"," +opa+","+oflags+",B," +odisplay +"," +right(awhen,3) +"," +oclearing+",D" 
													
			else

				w = left( awhen , 5 ) +",B," +str(tseq,9,0)    +"," +oport+"," +ouser +"," +otoken +"," +obuy_sell +",   " +str(oleaves,6,0) +",        0," +ostock +"," +str(oprice,11,4) +"," +str(tlocate,8,0) +"," +str(otif,5,0) +"," +oshort +"," +ommid +"," +opa+",    , ," +odisplay +"," +right(awhen,3) +"," +oclearing+",D"
				
			endif

			do write with w		
			
		endif		&& otif == 0
		
	endif  && oleaves > 0 
		
	if oleaves > 0 		&& Still open?
	
		goto tlocate
						
		replace ;
			PORT	with oport,;
			USER	with ouser,;
			TOKEN	with otoken,;
			BUY_SELL with obuy_sell,;
			LEAVES 	with oleaves,;
			STOCK 	with ostock,;
			PRICE 	with oprice,;
			SHORT	with oshort,;
			MMID 	with ommid,;
			PA		with opa,;
			DISPLAY	with odisplay,;
			CLEARING with oclearing,;
			FILLED	with ofilled,;
			SEQ	with tseq
					
	else && leaves = 0 (add current order to deadlist)
	
		goto tlocate
		replace SEQ with tnextdead
		tnextdead = recno()
	
	endif && oleaves > 0 	
			
return

****** Enter Report adds a type R line to the file
****** Enter order assumes ostock and obuy_sell are the right length.

proc enter2report

	parameter oport,ouser,otoken,obuy_sell,oshares,ostock,oprice,oshort,ommid,opa,oreport,oclearing,ocontra
		
	tseq = tseq + 1	
						   
	tmatch = tmatch + 1

	mwhen = str( tnow , 9 , 3)
	
	w = left( mwhen , 5 ) + ",R,"+str(tseq,9,0) +"," +oPORT+"," +oUSER+"," +oTOKEN+"," +oBUY_SELL +",   "+str(oshares,6,0)+","+str(tmatch,9,0)+"," +oSTOCK+","+str(oprice,11,4)+",00000000,    0,"+oSHORT+","+oMMID+","+oPA+","+ocontra+","+oreport+",R,"+right(mwhen,3)+","+oCLEARING+",D"

	do write with w
	
return


****** Enter Reject adds a type J line to the file

proc enter2reject

	parameter oport,ouser,otoken,otype,oreason
			
	tseq = tseq + 1	
						   
	*** Leaves MUST be 0 or it might be included in the match!
									 	
	mwhen = str( tnow , 9 , 3)
	
	w = left( mwhen , 5 ) + ",J," +str(tseq,9,0) +"," +oport+"," +ouser +"," +otoken+"," +otype+",        0,        0,      ,     0.0000,00000000,    0, ,    , ,    ," +oreason +",N,"+right(mwhen,3)+", ,D"

	do write with w
	
return


****** Enter break adds a type C line to the file

proc enterbreak

	parameter oseq, oport , ouser, otoken, obuy_sell, oshares, omatch, ostock, oprice, ommid, oreason, omisc, oclearing , oshort
	
	if oseq > tseq .or. oseq < 1
	
		? time()+" Bad SEQ in Break! OSEQ="+str(oseq,9,0)+" RECS="+str( tseq, 9,0)
		
		return
	
	endif
	
	if oshares <= 0 

		? time()+" Jerk boy BREAK zero shares SEQ="+str(oseq,9,0)
		
		return
		
	endif

	if oprice <= 0 
	
		? time()+" Jerk boy BREAK zero price SEQ="+str(oseq,9,0)
		
		return
		
	endif
	
	if ! omisc $ "AR"
	
		? time()+" Invalid A/R BREAK MISC="+omisc+" SEQ="+str(oseq,9,0)
		
		return
		
	endif
		
	mwhen = str( tnow , 9 , 3)
	
	w = left( mwhen , 5 ) + ",C,"+str(oseq,9,0) +"," +oPORT+"," +oUSER+"," +oTOKEN+"," +oBUY_SELL +",   "+str(oshares,6,0)+","+str(omatch,9,0)+"," +oSTOCK+","+str(oprice,11,4)+",00000000,    0,"+oshort+","+oMMID+", ,"+oreason +","+omisc+", ,"+right(mwhen,3)+","+oCLEARING+",D"

	do write with w
	
return


****** Enter maint adds a type M line to the file


*** SHORTTYPE
***** N - No short sale checks
***** B - Bidtick test
***** L - Lasttrade test

*** BLOCKSUB - Block subscriber only orders?
***** B - Block
***** N - No block

*** CENTER
***** Q - NASDAQ
***** L - LISTED

proc entermaint

	parameter oport,ouser,otoken,ostock,oshorttype,oblocksub,ocenter,omisc
	
	if ! oshorttype $ "NBL"
		
		? time()+" Bad SHORTTYPE="+oshorttype+" STOCK="+ostock
			
		return
			
	endif
	
	if !oblocksub $ "BN"
		? time()+" Bad BLOCKSUB="+oblocksub+" STOCK="+ostock
			
		return
	endif
	
	if !ocenter $ "QL"
		? time()+" Bad CENTER="+ocenter+" STOCK="+ostock
			
		return
	endif
	
	if len( omisc ) # 4 
		? time()+" Bad omisc len!"+omisc
		return
	endif
	
	for eml = 1 to 4
	
		emb = substr( omisc , eml , 1 )
	
		if !isalpha( emb ) .and. emb # " "
		
			? time()+" Bad omisc letter!"+omisc
			return
			
		endif
		
	endfor
	
	awhen = str( tnow , 9 , 3 )

	w = left( awhen , 5 ) +",M,        0,"+oport+","+ouser+","+otoken+",S,        0,        0," +ostock +",     0.0000,00000000,    0,"+oshorttype+",    , ,"+omisc+","+ocenter+","+oblocksub+"," +right(awhen,3) +", , "

	do write with w
	
return

*** STATE - Trading state
***** T - Trading
***** H - Halted

proc enterstate

	parameter oport,ouser,otoken,ostock,ostate
	
	if !ostate $ "TH"
		? time()+" Bad STATE="+ostate+" STOCK="+ostock
			
		return
	endif
	
	awhen = str( tnow , 9 , 3 )

	w = left( awhen , 5 ) +",M,        0,"+oport+","+ouser+","+otoken+",T,        0,        0," +ostock +",     0.0000,00000000,    0, ,    , ,    ,"+ostate+", ," +right(awhen,3) +", , "

	do write with w
	
return

*** Enter account configures an OUCH account

proc enteraccount

	parameter oaccount,opassword,otest,otrusted, othresh, osscheck,oiflag, odefault
	
	if !otest$"TN"
	
		? time()+" Bad TEST flag in enteraccount:"+otest
		return
		
	endif
	
	if !otrusted$"TN"
	
		? time()+" Bad TRUSTED flag in enteraccount:"+otrusted
		return
		
	endif
	
	if !osscheck$"YN"
	
		? time()+" Bad SSCHECK flag in enteraccount:"+osscheck
		return
		
	endif

	if !oiflag$"IN"
	
		? time()+" Bad IFLAG flag in enteraccount:"+oiflag
		return
		
	endif
		
	if len( opassword ) # 10
		? time()+" Bad opassword len!"+opassword
		return
	endif
	
	for eml = 1 to 10
	
		emb = substr( opassword , eml , 1 )
	
		if !isalpha( emb ) .and. !isdigit( emb) .and. emb # " " .and. emb # "!" .and. emb # "#"
		
			? time()+" Bad opassword letter!"+opassword+":"+str(eml)
			return
			
		endif
		
	endfor


	if len( oaccount ) # 6
		? time()+" Bad oaccount len!"+oaccount
		return
	endif
	
	for eml = 1 to 6
	
		emb = substr( oaccount , eml , 1 )
	
		if !isalpha( emb ) .and. !isdigit( emb ) .and. emb # " "
		
			? time()+" Bad oaccount letter!"+oaccount+":"+str(eml)
			return
			
		endif
		
	endfor

	if len( odefault ) # 4
		? time()+" Bad odefault len!"+odefault
		return
	endif
	
	for eml = 1 to 4
	
		emb = substr( odefault , eml , 1 )
	
		if !isalpha( emb ) .and. !isdigit( emb) .and. emb # " "
		
			? time()+" Bad odefault letter!"+odefault +":"+str(eml)
			return
			
		endif
		
	endfor
		
	awhen = str( tnow , 9 , 3 )

	w = left( awhen , 5 ) +",M,        0,"+oaccount+",    ,"+opassword+",A,"+str(othresh,9,0)+",        0,      ,     0.0000,00000000,    0,"+osscheck+","+odefault +", ,"+otest+ " "+ otrusted +" ,"+oiflag+", ," +right(awhen,3) +", , "

	do write with w
	
return


*** Enter firm configures an OUCH account for clearing

proc enterfirm

	parameter oaccount,ommid,oclearing
	
	if !oclearing$"AIQRN"
	
		? time()+" Bad clearing in enterfirm:"+oclearing
		return
		
	endif
			
	if len( ommid ) # 4
		? time()+" Bad ommid len!"+ommid
		return
	endif
	
	for eml = 1 to 4
	
		emb = substr( ommid , eml , 1 )
	
		if !isalpha( emb ) .and. !isdigit( emb) .and. emb # " "
		
			? time()+" Bad ommid letter!"+ommid+":"+str(eml)
			return
			
		endif
		
	endfor

	if len( oaccount ) # 6
		? time()+" Bad oaccount len!"+oaccount
		return
	endif
	
	for eml = 1 to 6
	
		emb = substr( oaccount , eml , 1 )
	
		if !isalpha( emb ) .and. !isdigit( emb ) .and. emb # " "
		
			? time()+" Bad oaccount letter!"+oaccount+":"+str(eml)
			return
			
		endif
		
	endfor
	
	
	awhen = str( tnow , 9 , 3 )

	w = left( awhen , 5 ) +",M,        0,"+oaccount+",    ,          ,F,        0,        0,      ,     0.0000,00000000,    0, ,"+ ommid +", ,    , , ," +right(awhen,3) +","+oclearing+", "

	do write with w
	
return

proc new2cancel

	parameter olocate,oseq,oshares,ocontra		&& Contra indeicates reason for cancel
	
	if olocate <= 0 .or. olocate > reccount()
		
		? time()+" CANCEL LOCATE OUT OF BOUNDS " +str( olocate , 8 , 0)+ " SEQ=" +str(oseq,8,0)
		
		return 0
	
	endif
	
	goto olocate

	if oseq # SEQ
		
***		? time()+" CANCEL OUT OF DATE " +str( olocate , 8 , 0)+ " SEQ=" +str(oseq,8,0)

		ttoolate = ttoolate + 1
		
		return 0
	
	endif
	
	if LEAVES <= oshares
	
		ttoolate = ttoolate + 1
	
		return
	
	endif
	
	xshares = LEAVES - oshares 

	do mwrite with "X", SEQ , xshares , PRICE , ocontra , " " , 0 , recno() 

	txcount = txcount + 1
	
	dleaves = LEAVES - xshares
	
	if dleaves <=0 
	
		&& Must set LEAVES to zero to pull order from indexes
		
		replace LEAVES with 0 , SEQ with tnextdead
		tnextdead = recno()
		
	else
	
		replace LEAVES with dleaves
		
	endif
		
return


****** Cancel Order takes a locator and a SEQ number
****** passed shares in new intedned order size
****** returns number of shares canceled and leaves record pointer on order canceled.

proc i2cancel

	parameter olocate,oseq,oshares,ocontra		&& Contra indeicates reason for cancel

	
	if olocate <= 0 .or. olocate > reccount()
		
		? time()+" CANCEL LOCATE OUT OF BOUNDS " +str( olocate , 8 , 0)+ " SEQ=" +str(oseq,8,0)
		
		return 0
	
	endif
	
	goto olocate

	if oseq # SEQ
		
***		? time()+" CANCEL OUT OF DATE " +str( olocate , 8 , 0)+ " SEQ=" +str(oseq,8,0)

		ttoolate = ttoolate + 1
		
		return 0
	
	endif
	
	msize = LEAVES + FILLED		&& Current size of order
		
	if oshares >= msize
	
***		? time()+" INTENEDED CANCEL BIGGER THAN ORDER SHARES="+str( oshares, 6 , 0 )+" REASON="+ocontra
		
		return 0
	
	endif

	xshares = msize - oshares		&& HOwmany do we have to cancel to get from current size to new size?
	
	if xshares > LEAVES				&& Don't cancel more than are open
	
		xshares = LEAVES
		
	endif
	
	if xshares = 0

		ttoolate = ttoolate + 1
		
		return 0
	
	endif

	do mwrite with "X", SEQ , xshares , PRICE , ocontra , " " , 0 , recno()

	txcount = txcount + 1
		
	dleaves = LEAVES - xshares
	
	if dleaves <=0 
	
		&& Must set LEAVES to zero to pull order from indexes
		
		replace LEAVES with 0 , SEQ with tnextdead
		tnextdead = recno()
		
	else
	
		replace LEAVES with dleaves
		
	endif
				
return xshares


proc initstatus
	activate window status
	@ 0,1  SAY "Orders:"
	@ 0,27 SAY "Executions:"
	@ 0,52 SAY "Cancels:"
	activate window ticker
return

proc showstatus
	activate window status
	@ 0,11 SAY tseq    PICTURE "##,###,###"
	@ 0,38 SAY tmatch  PICTURE "##,###,###"
	@ 0,60 SAY txcount PICTURE "##,###,###"
	activate window ticker
return

proc processtcpip

	param ptm
	
	tcommand = substr( ptm , 1 , 1 )
		
	if geom
	
		? time()+" Command recieved after EOM:"+tcommand
		return
	
	endif
		
	do case
		
		case tcommand = "H"
				
			tport 		= 		substr( ptm ,  2 , 6  )
						
			if !seek( tport , "SOURCES" ) 
			
				insert into SOURCES values ( tport , 1 )
				
			endif
			
			tinseq      =   val(substr( ptm , 73,  9  ) )
			
			if tinseq # SOURCES.SEQ
			
				if tinseq < SOURCES.SEQ 
			
***					? time()+" Duped inseq "+tport+" Expected:"+str( SOURCES.SEQ , 9, 0)+" Got:"+str( tinseq , 9 , 0 )				
					return

				endif			
				
				do alert with "Gapped inseq "+tport+" Expected:"+str( SOURCES.SEQ , 9, 0)+" Got:"+str( tinseq , 9 , 0 )				
				
			endif				
									
			tuser 		= 		substr( ptm ,  8 , 4  )
			ttoken 		= 		substr( ptm , 12 , 10 )
			tbuy_sell	= 		substr( ptm , 22 , 1  )
			tshares 	= val(	substr( ptm , 23 , 6  ) )
			tmin		= val(	substr( ptm , 29 , 6  ) )
			tstock		= 		substr( ptm , 35 , 6  )
			tprice 		= val(  substr( ptm , 41 , 11 ) )
			ttif		= val( 	substr( ptm , 52 , 5  ) )
			tdisplay    =		substr( ptm , 57 , 1  )
			tshort		=		substr( ptm , 58 , 1  )
			tpa			=		substr( ptm , 59 , 1  )
			tmmid		=		substr( ptm , 60 , 4  )
			tclearing  	= 		substr( ptm , 64 , 1  )
			tflags      = 		substr( ptm , 65 , 4  )
						
			if tbuy_sell = "B"
			
				xshort = " "
				
			else
			
				if tbuy_sell # "S"

					? time()+" Bad buy_sell "+tbuy_sell+" port="+tport
					
					return
					
				endif
			
				do case
					case tshort = "Y"
						xshort = "S"
						
					case tshort = "S"
						xshort = "S"
						
					case tshort = "N"
						xshort = "L"
						
					case tshort = "L"
						xshort = "L"	
						
					case tshort = "E"
						xshort = "E"
					
					otherwise 
						xshort = "?"
						
				endcase
				
			endif

			if tshares <= 0 

				? time()+" Jerk boy zero shares TOKEN " +tport+"-" +tuser+"-" +ttoken
		
				return
		
			endif

			if tshares >= 1000000

				? time()+" Jerk boy million shares TOKEN " +tport+"-" +tuser+"-" +ttoken
		
				return
		
			endif


			if tprice <= 0 
	
				? time()+" Jerk boy zero price TOKEN " +tport+"-" +tuser+"-" +ttoken
		
				return
		
			endif
	
	
			if tprice >= 1000000
	
				? time()+" Jerk boy million price TOKEN " +tport+"-" +tuser+"-" +ttoken
		
				return
		
			endif
		
	
			if ! tclearing $ "AIQOR"
	
				? time()+" Jerk boy strange clearing >" +tclearing+"< TOKEN " +tport+"-" +tuser+"-" +ttoken
		
				return
		
			endif


			if ! tdisplay $ "YNLRP"
	
				? time()+" Jerk boy strange display >" +tdisplay+"< TOKEN " +tport+"-" +tuser+"-" +ttoken
		
				return
		
			endif
			
			if (tdisplay = "R" .or. tdisplay = "P" ) .and. ttif > 0
			
				? time()+" Jerk boy R or P or Q display with nonzero tif >" +str(ttif,5,0)+"< TOKEN " +tport+"-"+tuser+"-"+ttoken
		
				return
		
			endif	
		
			if !gaccept
	
				? time()+" Trade ignored outside SOD-EOD time "+tport+"-" +tuser+"-" +ttoken
		
				return
			
			endif
	
			if tclearing = "A"
	
				if !gactaccept
	
					? time()+" ACT trade ignored outside ACT time " +tport+"-" +tuser+"-" +ttoken
		
					return
			
				endif
				
			endif

			
			select SOURCES
			replace SEQ with tinseq + 1
			select ISLAND
														
			do enter2order with tport , tuser , ttoken , tbuy_sell , tshares , tstock , tprice , ttif , xshort , tmmid , tpa , tdisplay , tmin , tclearing , tflags
							
		case tcommand = "L"

			tolocate	= val(	substr( ptm ,  2 , 8  ) )
			toseq 		= val(	substr( ptm , 10 , 9  ) )
			tshares 	= val(	substr( ptm , 19 , 6  ) )
			treason		= 		substr( ptm , 25 , 1  ) 
			
			do case
				case treason = "A"
					xreason = "#USR"
				case treason = "B"
					xreason = "#TME"
				case treason = "C"
					xreason = "#HLT"
				case treason = "D"
					xreason = "#SUP"
				case treason = "E"
					xreason = "#DNT"
				case treason = "F"
					xreason = "#MBL"
					
				otherwise
					xreason = "#USR"
			endcase
							
			do new2cancel with tolocate , toseq , tshares , xreason
			
		case tcommand = "I"

			tport 		= 		substr( ptm ,  2 , 6  )
			
									
			if !seek( tport , "SOURCES" ) 
			
				insert into SOURCES values ( tport , 1 )
				
			endif
			
			tinseq 		=  val( substr( ptm , 24 , 9 ) )			
			
			if tinseq # SOURCES.SEQ
			
				if tinseq < SOURCES.SEQ 
			
***					? time()+" Duped REJ inseq "+tport+" Expected:"+str( SOURCES.SEQ , 9, 0)+" Got:"+str( tinseq , 9 , 0 )				
					return

				endif			
				
				do alert with "Gapped REJ inseq "+tport+" Expected:"+str( SOURCES.SEQ , 9, 0)+" Got:"+str( tinseq , 9 , 0 )				
				
			endif				
												
			
			tuser 		= 		substr( ptm ,  8 , 4  )
			ttoken 		= 		substr( ptm , 12 , 10 )
			ttype		=		substr( ptm , 22 , 1 )
			treason		= 		substr( ptm , 23 , 1  ) 			
			
			do enter2reject with tport , tuser , ttoken , ttype , treason

			select SOURCES
			replace SEQ with tinseq + 1
			select ISLAND

		case tcommand = "Z"

			tolocate	= val(	substr( ptm ,  2 , 8  ) )
			toseq 		= val(	substr( ptm , 10 , 9  ) )
			tshares 	= val(	substr( ptm , 19 , 6  ) )
			treason		= 		substr( ptm , 25 , 1  ) 
			
			do case
				case treason = "A"
					xreason = "#USR"
				case treason = "B"
					xreason = "#TME"
				case treason = "C"
					xreason = "#HLT"
				case treason = "D"
					xreason = "#SUP"
				case treason = "E"
					xreason = "#DNT"
				case treason = "F"
					xreason = "#MBL"
					
				otherwise
					xreason = "#USR"
			endcase
					
		
			=i2cancel( tolocate , toseq , tshares , xreason )
			
		case tcommand = "S"

			tport 		= 		substr( ptm ,  2 , 6  )
			
			if !seek( tport , "SOURCES" ) 
			
				insert into SOURCES values ( tport , 1 )
				
 			endif
			
			tinseq        =   val(substr( ptm , 58 , 9  ) )
			
			if tinseq # SOURCES.SEQ
			
				if tinseq < SOURCES.SEQ 
			
***					? time()+" Duped REP inseq "+tport+" Expected:"+str( SOURCES.SEQ , 9, 0)+" Got:"+str( tinseq , 9 , 0 )				
					return

				endif			
				
				do alert with "Gapped REP inseq "+tport+" Expected:"+str( SOURCES.SEQ , 9, 0)+" Got:"+str( tinseq , 9 , 0 )				
				
			endif				
		
			tuser 		= 		substr( ptm ,  8 , 4  )
			ttoken 		= 		substr( ptm , 12 , 10 )
			tbuy_sell	= 		substr( ptm , 22 , 1  )
			tshares 	= val(	substr( ptm , 23 , 6  ) )
			tstock		= 		substr( ptm , 29 , 6  )
			tprice 		= val(  substr( ptm , 35 , 11 ) )
			treport 	=		substr( ptm , 46 , 1  ) 
			tshort		=		substr( ptm , 47 , 1  )
			tpa			=		substr( ptm , 48 , 1  )
			tmmid		=		substr( ptm , 49 , 4  )
			tclearing  	= 		substr( ptm , 53 , 1  )
			tcontra     =       substr( ptm , 54 , 4  )
								
	
			if ! treport $ "YNS"
				? time()+" Jerk boy REPORT code TOKEN " +tport
				
				return
		
			endif

	
			if tshares <= 0 

				? time()+" Jerk boy REPORT zero shares TOKEN "+tport+"-"+tuser+"-"+ttoken
		
				return
		
			endif


			if tshares >= 1000000

				? "Jerk boy REPORT million shares TOKEN "+tport+"-"+tuser+"-"+ttoken
		
				return
		
			endif


			if tprice <= 0 
	
				? "Jerk boy REPORT zero price TOKEN "+tport+"-"+tuser+"-"+ttoken
		
				return
		
			endif

			if tprice >= 1000000
	
				? "Jerk boy REPORT million price TOKEN "+tport+"-"+tuser+"-"+ttoken
		
				return
		
			endif

	
			if ! tclearing $ "ABSIQORN"
	
				? "Jerk boy REPORT strange clearing >"+tclearing+"< TOKEN "+tport+"-"+tuser+"-"+ttoken
		
				return
		
			endif
	
			if !gaccept
	
				? "Report ignored outside SOD-EOD time "+tport+"-"+tuser+"-"+ttoken
		
				return
			
			endif
	
			if tclearing = "A"
	
				if !gactaccept
		
					? "ACT trade REPORT ignored outside ACT time "+tport+"-"+tuser+"-"+ttoken
		
					return
			
				endif
		
			endif

			do enter2report with tport , tuser , ttoken , tbuy_sell , tshares , tstock , tprice , tshort , tmmid , tpa , treport ,  tclearing , tcontra

			select SOURCES
			replace SEQ with tinseq + 1
			select ISLAND

		case tcommand = "D"

			tcseq 		= val(  substr( ptm ,  2 , 9  ) )
			tcport		= 		substr( ptm , 11 , 6 )
			tcuser		=		substr( ptm , 17 , 4 )
			tctoken 	=		substr( ptm , 21 , 10 )
			tcbuy_sell  =       substr( ptm , 31 , 1  )
			tcshares 	= val(	substr( ptm , 32 , 6  ) )
			tcmatch     = val(  substr( ptm , 38 , 9  ) )
			tcstock     =       substr( ptm , 47 , 6 )
			tcprice 	= val(  substr( ptm , 53 , 11 ) )
			tcmmid   	=       substr( ptm , 64 , 4  )
			tcmisc		=       substr( ptm , 68 , 1 )
			tcreason	= 		substr( ptm , 69 , 1 ) 
			tcclearing	=       substr( ptm , 70 , 1 )
			
			do case
				case tcreason = "A"
					xreason = "#ERR"
				case tcreason = "B"
					xreason = "#CON"
				case tcreason = "C"
					xreason = "#SUP"
				case tcreason = "D"
					xreason = "#SYS"
				case tcreason = "E"
					xreason = "#EXT"
					
				otherwise
				
					? time()+" Unvalid break reason = "+tcreason
					
					return
					
			endcase
			
			do case
			
				case tcbuy_sell = "B"
					xbuy_sell = "B"
					xshort    = " "
					
				case tcbuy_sell = "S"
					xbuy_sell = "S"
					xshort = "L"
					
				case tcbuy_sell = "T"
					xbuy_sell = "S"
					xshort = "S"
					
				case tcbuy_sell = "E"
					xbuy_sell = "S"
					xshort = "E"
					
				otherwise
				
					? time()+" Unvalid break buy_sell = "+tcbuy_sell
					return
					
			endcase
													
			do enterbreak with tcseq , tcport, tcuser, tctoken, xbuy_sell, tcshares , tcmatch ,tcstock , tcprice , tcmmid , xreason , tcmisc , tcclearing , xshort

		case tcommand = "M"
		
			tctype 		= 		substr( ptm ,  2 , 1 )
			
			do case
			
				case tctype = "S"		&& Stock maintence

					tport 		= 		substr( ptm ,  3 , 6  )
					tuser 		= 		substr( ptm ,  9 , 4  )
					ttoken 		= 		substr( ptm , 13 , 10 )
						
					tcstock     = 		substr( ptm , 23 , 6 )
				
					tcshorttype = substr( ptm , 29 , 1 )
					tcblocksub  = substr( ptm , 30 , 1 )
					tccenter    = substr( ptm , 31 , 1 )
					tcmisc		= substr( ptm , 32 , 4 )

					do entermaint with tport, tuser, ttoken, tcstock , tcshorttype , tcblocksub , tccenter , tcmisc

				case tctype = "A"		&& Account settings

					taccount	= 		substr( ptm ,  3 , 6  )
					tpassword	= 		substr( ptm ,  9 , 10 )
					ttest       =       substr( ptm , 19 , 1  )
					ttrusted    =       substr( ptm , 20 , 1  ) 
					tthresh     = val(  substr( ptm , 21 ,  6 ) )
					tsscheck    =       substr( ptm , 27 , 1 )
					tiflag      =       substr( ptm , 28 , 1 )
					tdefault    =       substr( ptm , 29 , 4  )
					
					do enteraccount with taccount, tpassword, ttest , ttrusted , tthresh, tsscheck, tiflag, tdefault
				
				case tctype = "F"		&& Account settings

					taccount	= 		substr( ptm ,  3 , 6  )
					tmmid       =       substr( ptm ,  9 , 4  )
					tclearing   =       substr( ptm , 13 , 1  )
					
					do enterfirm with taccount, tmmid, tclearing
					
				case tctype = "T"		&& Stock state

					tport 		= 		substr( ptm ,  3 , 6  )
					tuser 		= 		substr( ptm ,  9 , 4  )
					ttoken 		= 		substr( ptm , 13 , 10 )
						
					tcstock     = 		substr( ptm , 23 , 6 )
				
					tcstate     = substr( ptm , 29 , 1 )
					
					do enterstate with tport, tuser, ttoken, tcstock , tcstate
			
				otherwise
							
					? time()+" Unvalid maint reason = "+tctype
					
				
			endcase
						
		otherwise
			
			do alert with "Unknown command"+ptm
				
	endcase
				
return

proc sendeos

	s = "E"			
	call int99 with s

	if s # "f" .and. s # "b"

		activate window output		
	
		? time() +" !!!! ERROR ON EOS WRITE!!!!!!"
			
		? ws

		activate window ticker
				
		suspend
		
	endif
			
return


proc flush

	s = "F"			
	call int99 with s			
	if s # "f"
		? "!!!!ERROR ON FLUSH!!!!"
		
		do alert with "Error on flush:"+werror(s)
		suspend
	endif
		
	gstoredflag = .F.
		
return

proc shutdown

	do alert with "Shutdown initiated"
	
	if !geom	&& Shutting down too early? Just in case...
	
		wait window "Can't end day, EOM has not happened yet!" nowait
		
		do alert with "Ending day attempted before EOM time!"
		
		return
		
	endif
	
		
	sdk = chr( (rand() * 25) + asc("A") )

	wait window "Initiate Shutdown by pressing ["+sdk+"] within 10 seconds" to sdh timeout 10
	
	if upper( sdh ) # sdk 
	
		wait window "Shutdown Aborted" nowait
		
		do alert with "Shutdown aborted"
		
		return 
		
	endif
	
	wait window "Confirm Shutdown by pressing the magic key within 10 seconds" to sdh timeout 10 
	
	if upper( sdh ) # "M"
	
		wait window "Inccorect Shutdown confirmation" nowait
		
		do alert with "Incorrect shutdown magic key"
		
		return
		
	endif

	do alert with "Shutdown confirmed"
	
	if file( mholdofffile )
	
		? "Holdoff file exists! Escalate!"
		suspend
		
	else
	
		sdf = fcreate( mholdofffile )
		=fputs( sdf , "Stop in the name of love!")
		=fclose( sdf )
		
	endif

	if !file( mholdofffile )
	
		wait window "No HOLDOFF file created, aborting Shutdown" nowait
		
		do alert with "No HOLDOFF file created, aborting shutdown"
		
		return 
		
	endif
		
	wait window "Shutting down..." nowait
	
	do swrite with "N" , ""
	wait window "Shutting down... 3" timeout 1

	do swrite with "N" , ""
	wait window "Shutting down... 2" timeout 1

	do swrite with "N" , ""
	wait window "Shutting down... 1" timeout 1
	
	do flush
	
	wait window "Flushing..." timeout 1
	
	do sendeos
	do flush

	wait window "Sending End of Session... 3" timeout 1
	do flush
	
	wait window "Sending End of Session... 2" timeout 1
	do flush
	
	wait window "Sending End of Session... 1" timeout 1
	do flush
	
	wait window "Sending End of Session... 0" timeout 1
	do flush
	
	mdone = .T.
	
	zap
	
return 


proc pingreply 
	param preply , ptm
		
	ptoken  = substr( ptm , 2 , 12 )
	pport = substr( ptm , 14 , 4 )
	
	x = "R"+ptoken+str( tseq ,9,0)+str( tmatch ,9,0)+str(0,9,0)	

	
	s = "S"+chr(len(x))+ preply+pport+ x
	call int99 with s
	
return


proc seteventflags
	param secode
	
	do case
	
		case secode = "SOD"
		
			gaccept = .T.
			gactaccept = .T.
			
		case secode = "EOA"
		
			gactaccept = .F.
			
		case secode = "EOD"
		
			gaccept = .F.
						
		case secode = "EOM"
		
			geom = .T.
	endcase
	
return

proc alert
parameter s
	xs = dtoc(date())+" "+time() +"-"+ s
	
	? xs
	
	IF FILE( malertfile ) && Does file exist? 
		errfile = FOPEN( malertfile ,12) && If so, open read/write
	ELSE
		errfile = FCREATE( malertfile )		&& If not create it
	ENDIF
	
	IF errfile < 0 && Check for error opening file
		WAIT 'Cannot open or create output file' WINDOW NOWAIT
	ELSE && If no error, write to file
		=fseek( errfile, 0 , 2 )
		=FWRITE(errfile, xs + chr(13) + chr(10) )
	ENDIF

	=FCLOSE(errfile) && Close file
	
	*	? chr(07)+chr(07)+chr(07)+chr(07)
return