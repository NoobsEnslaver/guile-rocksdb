(test-group "options"
 (let ([tmp1 (make-tmp-dir)]
       [dbopts (rocksdb-options-create)])
   (rocksdb-options-set-create-if-missing! dbopts 1)
   (let ([db (rocksdb-open dbopts tmp1)])
     "just open (new) and close db to create OPTIONS file")

   (define (options->alist opt)
     (rocksdb-options-string->alist (rocksdb-options->string opt)))

   (define (alist->options-string alist)
     (string-append
      (string-join
       (map (lambda (x)
              (string-join (list (car x) (cdr x)) "=")) alist)
       ";") ";"))


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
                        (equal? (class-of dbopts) (class-of opts))
                        (equal? (rocksdb-options-get-info-log-level dbopts)
                                (rocksdb-options-get-info-log-level opts))))
      (test-assert (and (list? cf-list)
                        (every (lambda(x)
                                 (and (string? (car x))
                                      (and (instance? (cdr x))
                                           (equal? (class-of dbopts)
                                                   (class-of (cdr x)))
                                           (equal? (rocksdb-options-get-info-log-level dbopts)
                                                   (rocksdb-options-get-info-log-level (cdr x))))))
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
                                                   (class-of (cdr x)))
                                           (equal? (rocksdb-options-get-info-log-level dbopts)
                                                   (rocksdb-options-get-info-log-level (cdr x))))))
                               cf-list)))))

   (test-group
    "options:rocksdb-set-options"
    (let* ([dbopts (rocksdb-options-create)]
           [_ (rocksdb-options-set-create-if-missing! dbopts 1)]
           [db (rocksdb-open dbopts (make-tmp-dir))]
           [old-opts (options->alist (rocksdb->rocksdb-options db))]
           [opts-diff '(["write_buffer_size" . "129K"]
                        ["max_write_buffer_number" . "7"])]
           [expected-buf-size (number->string (* 129 1024))])
      (rocksdb-set-options db opts-diff)
      (define new-opts (options->alist (rocksdb->rocksdb-options db)))
      (test-not-equal (assoc-ref old-opts "write_buffer_size")
                      (assoc-ref new-opts "write_buffer_size"))
      (test-not-equal (assoc-ref old-opts "max_write_buffer_number")
                      (assoc-ref new-opts "max_write_buffer_number"))
      (test-equal expected-buf-size (assoc-ref new-opts "write_buffer_size"))
      (test-equal "7" (assoc-ref new-opts "max_write_buffer_number"))
      (test-equal `(("write_buffer_size" . ,expected-buf-size) ("max_write_buffer_number" . "7"))
        (lset-difference equal? new-opts old-opts))))

   (test-group
    "options:rocksdb-options->string + string->rocksdb-options"
    (let* ([dbopts (rocksdb-options-create)]
           [dbopts-str (rocksdb-options->string dbopts)]
           [dbopts-2 (string->rocksdb-options dbopts-str)]
           [dbopts-str-2 (rocksdb-options->string dbopts-2)]
           [tokens-1 (rocksdb-options-string->alist dbopts-str)]
           [tokens-2 (rocksdb-options-string->alist dbopts-str-2)])
      (test-assert (and (string? dbopts-str) (string? dbopts-str-2)))
      (test-equal dbopts-str dbopts-str-2)
      (test-assert (alist=? tokens-1 tokens-2))

      (define tokens-3 (assoc-set! tokens-2 "stats_dump_period_sec" "791"))
      (define dbopts-str-3 (alist->options-string tokens-3))
      (define dbopts-3 (string->rocksdb-options dbopts-str-3))
      (define tokens-4 (rocksdb-options-string->alist (rocksdb-options->string dbopts-3)))
      (test-assert (alist=? tokens-3 tokens-4))
      (test-equal (assoc-ref tokens-4 "stats_dump_period_sec") "791")))

   ))
