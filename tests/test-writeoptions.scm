(test-group
 "writeoptions"
 (let ([wopt1 (rocksdb-writeoptions-create :sync #t
                                           :disable-wal #f
                                           :ignore-missing-column-families #t
                                           :no-slowdown #t
                                           :low-pri #t
                                           :memtable-insert-hint-per-batch #t)]
       [wopt2 (rocksdb-writeoptions-create :sync #t
                                           :disable-wal #f
                                           :ignore-missing-column-families #f
                                           :no-slowdown #f
                                           :low-pri #f
                                           :memtable-insert-hint-per-batch #f)]
       [wopt1-expected '([sync . #t]
                         [disable-wal . #f]
                         [ignore-missing-column-families . #t]
                         [no-slowdown . #t]
                         [low-pri . #t]
                         [memtable-insert-hint-per-batch . #t])]
       [wopt2-expected '([sync . #t]
                         [disable-wal . #f]
                         [ignore-missing-column-families . #f]
                         [no-slowdown . #f]
                         [low-pri . #f]
                         [memtable-insert-hint-per-batch . #f])])
   (test-assert (alist=? wopt1-expected (rocksdb-writeoptions->alist wopt1)))
   (test-assert (alist=? wopt2-expected (rocksdb-writeoptions->alist wopt2)))

   (with-db
    (λ (db)
      (rocksdb-put db #u8(1 1 1) #u8(2 2 2) wopt1)
      (rocksdb-put db #u8(3 3 3) #u8(4 4 4) wopt2)
      (test-equal #u8(2 2 2) (rocksdb-get db #u8(1 1 1)))
      (test-equal #u8(4 4 4) (rocksdb-get db #u8(3 3 3)))
      (rocksdb-delete db #u8(1 1 1) wopt1)
      (rocksdb-delete db #u8(3 3 3) wopt2)))

   (test-assert (alist=? wopt1-expected (rocksdb-writeoptions->alist wopt1)))
   (test-assert (alist=? wopt2-expected (rocksdb-writeoptions->alist wopt2)))
   )
 )
