(define (problem sokoban-2)
	(:domain sokoban-domain)
	(:objects player ; the dude
						A B    ; the blocks
						p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14) ; the locations
	(:init 
			; initialize object types
			(dude player)
			(block A) (block B)
			(space p1) (space p2) (space p3) (space p4) (space p5) (space p6)
			(space p7) (space p8) (space p9) (space p10) (space p11) (space p12) (space p13) (space p14)
			; initialize relationships between spaces
			; --- row relationships
			(is-left p1 p2) (is-left p2 p3)
			(is-left p4 p5) (is-left p5 p6) (is-left p6 p7)
			(is-left p8 p9) (is-left p9 p10)
			(is-left p11 p12) 
			; --- column relationships
			(is-above p1 p4) 
			(is-above p2 p5) (is-above p5 p8)
			(is-above p3 p6) (is-above p6 p9) (is-above p9 p11)
			(is-above p7 p10) (is-above p10 p12) (is-above p12 p13) (is-above p3 p14)
			; place the objects
			(at player p4)
			;(has-dude p4)
			(at A p5)
			(has-block p5)
			(at B p6)
			(has-block p6)
			; clear everything else 
			;(clear p1) (clear p2) (clear p3) (clear p7) (clear p8) (clear p9)
			;(clear p10) (clear p11) (clear p12) (clear p13) (clear p14)
			)
	(:goal (and 
						(at A p1)
						(has-block p1)
						(at B p14)
						(has-block p14))))