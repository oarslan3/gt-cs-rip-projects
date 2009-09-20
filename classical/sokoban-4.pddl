(define (problem sokoban-4)
	(:domain sokoban-domain)
	(:objects player ; the dude
						A B C   ; the blocks
						p1 p2 p3 p4 p5 p6 p7 p8 p9 p10
						p11 p12 p13 p14 p15 p16 p17 p18 p19) ; the locations
	(:init 
			; initialize object types
			(dude player)
			(block A) (block B) (block C)
			(space p1) (space p2) (space p3) (space p4) (space p5) 
			(space p6) (space p7) (space p8) (space p9) (space p10) 
			(space p11) (space p12) (space p13) (space p14) (space p15) 
			(space p16) (space p17) (space p18) (space p19) 
			; initialize relationships between spaces
			; --- row relationships
			(is-left p1 p2) 
			(is-left p3 p4)
			(is-left p5 p6) (is-left p6 p7) (is-left p7 p8) (is-left p8 p9) 
			(is-left p10 p11) (is-left p11 p12) (is-left p12 p13) (is-left p13 p14) 
			(is-left p15 p16) (is-left p16 p17) 
			(is-left p18 p19) 
			; --- column relationships
			(is-above p5 p10)
			(is-above p6 p11) (is-above p11 p15) (is-above p15 p18) 
			(is-above p1 p3) (is-above p3 p7) (is-above p7 p12) (is-above p12 p16) (is-above p16 p19) 
			(is-above p2 p4) (is-above p4 p8) (is-above p8 p13) (is-above p13 p17) 
			(is-above p9 p14) 
			; place the objects
			(at player p9)
			;(has-dude p9)
			(at A p3)
			(has-block p3)
			(at B p6)
			(has-block p6)
			(at C p15)
			(has-block p15)
			)
	(:goal (and 
						(has-block p11)
						(has-block p12)
						(has-block p13))))