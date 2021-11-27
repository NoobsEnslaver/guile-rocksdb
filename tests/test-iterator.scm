(test-group
 "iterator"
 (with-db
  (lambda (db)
    (do ([i 1 (+ i 1)]) ([> i 10])
      (rocksdb-put db (make-u8vector 5 i) (make-u8vector 10 i)))

    (define iter (rocksdb-create-iterator db))

    (test-group "invalid iterator"
     (test-assert (rocksdb-iterator? iter))
     (test-equal #f (rocksdb-iter-key iter))
     (test-equal #f (rocksdb-iter-value iter))
     (test-equal #f (rocksdb-iter-next! iter))
     (test-equal #f (rocksdb-iter-prev! iter))
     (test-equal #f (rocksdb-iter-get-error iter)))


    (test-group
     "basic iteration"
     (test-equal #t (rocksdb-iter-seek-to-first! iter))
     (do ([i 1 (+ i 1)]
          [succ #t (rocksdb-iter-next! iter)]
          [k (rocksdb-iter-key iter) (rocksdb-iter-key iter)]
          [v (rocksdb-iter-value iter) (rocksdb-iter-value iter)])
         ((not (and succ k v)))
       (test-equal (make-u8vector 5 i) k)
       (test-equal (make-u8vector 10 i) v)
       (test-equal #f (rocksdb-iter-get-error iter))))

    (test-group
     "reverse iteration"
     (test-equal #t (rocksdb-iter-seek-to-last! iter))
     (do ([i 10 (- i 1)]
          [succ (rocksdb-iter-prev! iter) (rocksdb-iter-prev! iter)]
          [k (rocksdb-iter-key iter) (rocksdb-iter-key iter)]
          [v (rocksdb-iter-value iter) (rocksdb-iter-value iter)])
         ((not (and succ k v)))
       (test-equal (make-u8vector 5 i) k)
       (test-equal (make-u8vector 10 i) v)
       (test-equal #f (rocksdb-iter-get-error iter))))

    (test-group
     "seek iteration"
     (test-equal #t (rocksdb-iter-seek! iter (make-u8vector 1 5)))
     (do ([i 5 (+ i 1)]
          [succ #t (rocksdb-iter-next! iter)]
          [k (rocksdb-iter-key iter) (rocksdb-iter-key iter)]
          [v (rocksdb-iter-value iter) (rocksdb-iter-value iter)])
         ((not (and succ k v)))
       (test-equal (make-u8vector 5 i) k)
       (test-equal (make-u8vector 10 i) v)
       (test-equal #f (rocksdb-iter-get-error iter))))

    (test-group
     "backward seek iteration"
     (test-equal #t (rocksdb-iter-seek-for-prev! iter (make-u8vector 1 6)))
     (do ([i 5 (- i 1)]
          [succ #t (rocksdb-iter-prev! iter)]
          [k (rocksdb-iter-key iter) (rocksdb-iter-key iter)]
          [v (rocksdb-iter-value iter) (rocksdb-iter-value iter)])
         ((not (and succ k v)))
       (test-equal (make-u8vector 5 i) k)
       (test-equal (make-u8vector 10 i) v)
       (test-equal #f (rocksdb-iter-get-error iter))))

    (test-group
     "seek unexisting key"
     (test-equal #t (rocksdb-iter-seek! iter (make-bytevector 5 99)))
     (test-equal #f (rocksdb-iter-get-error iter))
     (test-equal #f (rocksdb-iter-next! iter))
     (test-equal #f (rocksdb-iter-prev! iter))
     (test-equal #f (rocksdb-iter-key iter))
     (test-equal #f (rocksdb-iter-value iter))))))
