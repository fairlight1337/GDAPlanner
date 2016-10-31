(define (problem set-mealtable)
  (:domain tablesetting)
  (:objects plate0 - object
	    countertop-sink countertop-island table - location
            left right - hand)
  (:init (not (found plate0))
         (free-hand left)
         (free-hand right)
         (at plate0 ?x));;countertop-sink))
  (:goal (and (at plate0 table)))
  (:metric minimize (total-cost)))
