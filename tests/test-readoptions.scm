(test-group
 "readoptions"
 (let* ([readopt-alist `([verify-checksums . ,(> 0.5 (random:uniform))]
                         [fill-cache . ,(> 0.5 (random:uniform))]
                         [read-tier . ,(random 10)]
                         [tailing . ,(> 0.5 (random:uniform))]
                         [readahead-size . ,(random 1000)]
                         [prefix-same-as-start . ,(> 0.5 (random:uniform))]
                         [pin_data . ,(> 0.5 (random:uniform))]
                         [total-order-seek . ,(> 0.5 (random:uniform))]
                         [max-skippable-internal-keys . ,(random 100)]
                         [background-purge-on-iterator-cleanup . ,(> 0.5 (random:uniform))]
                         [ignore-range-deletions . ,(> 0.5 (random:uniform))])]
        [readopt (rocksdb-readoptions-create readopt-alist)])
   (test-assert (alist=? readopt-alist (rocksdb-readoptions->alist readopt)))
   ))
