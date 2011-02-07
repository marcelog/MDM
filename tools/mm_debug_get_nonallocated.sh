#!/bin/bash
tools/mm_debug_get_freed.sh > /tmp/freed
tools/mm_debug_get_allocated.sh > /tmp/allocated
for i in `cat /tmp/freed`; do grep ${i} /tmp/allocated > /dev/null; echo ${i}\:${?}; done | grep \:1
