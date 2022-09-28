############################################################
############################################################
#####
#####		Smart UUCP Mailer specification
#####
#####	The other end must speak domain-based  addresses for
#####	this to work.
#####
#####		@(#)suucpm.m4 1.7 89/01/23 SMI; from UCB 3.1 5/1/83
#####
############################################################
############################################################

Msuucp,	P=/usr/bin/uux, F=sDFMhuU, S=15, R=15
	A=uux - -r  -a$f $h!rmail ($u)

S15
R$*<@$+>$*		$@$1<@$2>$3			accept usual domain name
R$+			$:$1<@LOCAL>			stick on our host name
