(test-group "main"
 (let ([opts (rocksdb-options-create)]
       [wopts (rocksdb-writeoptions-create)]
       [ropts (rocksdb-readoptions-create)])
   (rocksdb-options-set-create-if-missing! opts 1)
   (let ([db (rocksdb-open opts (make-tmp-dir))])
     (rocksdb-put db #u8(1) #u8(2))
     (rocksdb-put db #u8(3) #u8(4) wopts)
     (test-assert (bytevector=? (rocksdb-get db #u8(1)) (rocksdb-get db #u8(1))))
     (test-equal #f (rocksdb-get db #u8(2)))
     (test-assert (not (bytevector=? (rocksdb-get db #u8(1)) (rocksdb-get db #u8(3)))))
     (rocksdb-delete db #u8(1))
     (rocksdb-delete db #u8(3) wopts)
     (test-equal #f (rocksdb-get db #u8(1)))
     (test-equal #f (rocksdb-get db #u8(3))))))
