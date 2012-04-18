sp := $(sp).x
dirstack_$(sp) := $(d)
d := $(dir)

dir := $(TOP)/test1/ta
include $(dir)/ta.mk

dir := $(TOP)/test1/tb
include $(dir)/tb.mk


d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
