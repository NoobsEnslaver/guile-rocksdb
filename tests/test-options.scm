(test-group "options"
 (let* ([tmp1 (make-tmp-dir)]
        [dbopts (rocksdb-options-create)])
   (rocksdb-options-set-create-if-missing! dbopts 1)
   (let ([db (rocksdb-open dbopts tmp1)])
     "just open (new) and close db to create OPTIONS file")

   (test-group
    "options:get-latest-options-filename file not exists"
    (receive (ret desc)
        (get-latest-options-filename "unexisting-path/")
      (test-equal #f ret)
      (test-equal "No options files found in the DB directory.: unexisting-path/" desc)))

   (test-group
    "options:get-latest-options-filename file"
    (receive (file-name undef)
        (get-latest-options-filename tmp1)
      (test-assert (and (string? file-name)
                        (string-prefix? "OPTIONS-" file-name)
                        (file-exists? (string-append tmp1 "/" file-name))))))

   (test-group
    "options:load-options-from-file unexisting file"
    (receive (result desc)
        (load-options-from-file "unexisting-file.kek")
      (test-equal #f result)
      (test-assert (string? desc))))

   (test-group
    "options:load-options-from-file"
    (receive (opts cf-list)
        (load-options-from-file "../tests/rocksdb-test.config")
      (test-assert (and (instance? opts)
                        (equal? (class-of dbopts) (class-of opts))))
      (test-assert (and (list? cf-list)
                        (every (lambda(x)
                                 (and (string? (car x))
                                      (and (instance? (cdr x))
                                           (equal? (class-of dbopts)
                                                   (class-of (cdr x))))))
                               cf-list))))
    (receive (opts cf-list)             ;same, but with explicit cache creation
        (load-options-from-file "../tests/rocksdb-test.config" (rocksdb-cache-create-lru 65536))
      (test-assert (and (instance? opts)
                        (equal? (class-of dbopts) (class-of opts))))
      (test-assert (and (list? cf-list)
                        (every (lambda(x)
                                 (and (string? (car x))
                                      (and (instance? (cdr x))
                                           (equal? (class-of dbopts)
                                                   (class-of (cdr x))))))
                               cf-list)))))

   (test-group
    "options:load-latest-options file not exists"
    (receive (ret desc)
        (load-latest-options "unexisting-path/")
      (test-equal #f ret)
      (test-equal "No options files found in the DB directory.: unexisting-path/" desc)))

   (test-group
    "options:load-latest-options"
    (receive (opts cf-list)
        (load-latest-options tmp1)
      (test-assert (and (instance? opts)
                        (equal? (class-of dbopts) (class-of opts))))
      (test-assert (and (list? cf-list)
                        (every (lambda(x)
                                 (and (string? (car x))
                                      (and (instance? (cdr x))
                                           (equal? (class-of dbopts)
                                                   (class-of (cdr x))))))
                               cf-list))))
    (receive (opts cf-list)             ;same, but with explicit cache creation
        (load-latest-options tmp1 (rocksdb-cache-create-lru 65536))
      (test-assert (and (instance? opts)
                        (equal? (class-of dbopts) (class-of opts))))
      (test-assert (and (list? cf-list)
                        (every (lambda(x)
                                 (and (string? (car x))
                                      (and (instance? (cdr x))
                                           (equal? (class-of dbopts)
                                                   (class-of (cdr x))))))
                               cf-list)))))
   ))
