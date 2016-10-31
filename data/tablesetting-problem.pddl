(define (problem set-mealtable)
  (:domain tablesetting)
  (:objects plate0 plate1 - object ;; Polymorphism via ontology maybe?
	    cup0 cup1 - object
	    countertop-sink countertop-island table - location
	    left right - hand)
  (:init (not (found plate0))
	 (not (found plate1))
	 (not (found cup0))
	 (not (found cup1))
	 (free-hand left)
	 (free-hand right))
  (:goal (and (at plate0 table)
	      (at plate1 table)
	      (at cup0 table)
	      (at cup1 table)))
  (:metric minimize (total-cost)))
