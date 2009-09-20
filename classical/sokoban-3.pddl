(define (problem sokoban-3)
	(:domain sokoban-domain)
	(:objects player ; the dude
						A B C   ; the blocks
						p1 p2 p3 p4 p5 p6 p7 p8 p9 p10
						p11 p12 p13 p14 p15 p16 p17 p18 p19 p20
						p21 p22 p23 p24 p25 p26 p27) ; the locations
	(:init 
			; initialize object types
			(dude player)
			(block A) (block B) (block C)
			(space p1) (space p2) (space p3) (space p4) (space p5) 
			(space p6) (space p7) (space p8) (space p9) (space p10) 
			(space p11) (space p12) (space p13) (space p14) (space p15) 
			(space p16) (space p17) (space p18) (space p19) (space p20) 
			(space p21) (space p22) (space p23) (space p24) (space p25) 
			(space p26) (space p27)
			; initialize relationships between spaces
			; --- row relationships
			(is-left p1 p2) (is-left p2 p3) 
			(is-left p8 p9) (is-left p9 p10) (is-left p10 p11) (is-left p11 p12) (is-left p12 p13) (is-left p13 p14) (is-left p14 p15) (is-left p15 p16) 
			(is-left p21 p22) (is-left p22 p23) (is-left p23 p24) (is-left p24 p25) (is-left p25 p26) (is-left p26 p27) 
			; --- column relationships
			(is-above p8 p17) (is-above p17 p21) 
			(is-above p10 p18) (is-above p18 p23) 
			(is-above p12 p19) (is-above p19 p25) 
			(is-above p1 p4) (is-above p4 p6) (is-above p6 p14) (is-above p14 p20) (is-above p20 p27)
			(is-above p3 p5) (is-above p5 p7) (is-above p7 p16) 
			; place the objects
			(at player p9)
			(has-dude p9)
			(at A p15)
			(has-block p15)
			(at B p13)
			(has-block p13)
			(at C p11)
			(has-block p11)
			)
	(:goal (and 
						(at A p1)
						(has-block p1)
						(at B p4)
						(has-block p4)
						(at C p6)
						(has-block p6))))