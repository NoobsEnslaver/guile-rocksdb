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
              (define-values (k v) (car+cdr x))
              (when (string-contains v ";")
                (set! v (string-append "{" v "}")))
              (string-join (list k v) "="))
            alist)
       ";") ";"))

   (define (option-file->alist dir)
     (define-values (file-name e) (get-latest-options-filename dir))
     (unless file-name (error e))
     (with-input-from-file (string-append dir "/" file-name)
       (λ ()
         (do ([acc '()]
              [on 'root]
              [line (read-line) (read-line)])
             ((eof-object? line) acc)
           (set! line (string-trim-both line))
           (cond
            [(string-prefix? "#" line)] ;skip
            [(string= "" (string-trim-both line))
             (set! on 'root)]
            [(string-prefix? "[" line)
             (set! on line)
             (set! acc (assoc-set! acc on '()))]
            [else (let ([line (string-split line #\=)]
                        [sub-acc (assoc-ref acc on)])
                    (assoc-set! acc on (assoc-set! sub-acc (car line) (cadr line))))])))))

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
    "options:get-latest-options-filename file env"
    (receive (file-name undef)
        (get-latest-options-filename tmp1 (rocksdb-create-default-env))
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
        (load-latest-options tmp1 (rocksdb-cache-create-lru 65536 'guile))
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

      (define tokens-3 (assoc-set! tokens-2 "num_levels" "8"))
      (define dbopts-str-3 (alist->options-string tokens-3))
      (define dbopts-3 (string->rocksdb-options dbopts-str-3))
      (define tokens-4 (rocksdb-options-string->alist (rocksdb-options->string dbopts-3)))
      (test-assert (alist=? tokens-3 tokens-4))
      (test-equal (assoc-ref tokens-4 "num_levels") "8")))

   (test-group
    "options:rocksdb-options-create with keys"
    (let* ([dbopts (rocksdb-options-create :create-if-missing #t
                                           :info-log-level 3
                                           :write-buffer-size 65536
                                           :num-levels 7
                                           :max-bytes-for-level-base (* 128 1024 1024)
                                           :max-bytes-for-level-multiplier 5.1
                                           :level-compaction-dynamic-level-bytes #f
                                           :max-subcompactions 11
                                           :max-background-jobs 12)]
           [dbopts-alist (options->alist dbopts)])
      (test-assert (rocksdb-options? dbopts))
      (test-equal "true" (assoc-ref dbopts-alist "create_if_missing"))
      (test-equal "ERROR_LEVEL" (assoc-ref dbopts-alist "info_log_level"))
      (test-equal "65536" (assoc-ref dbopts-alist "write_buffer_size"))
      (test-equal "7" (assoc-ref dbopts-alist "num_levels"))
      (test-equal (number->string (* 128 1024 1024)) (assoc-ref dbopts-alist "max_bytes_for_level_base"))
      ;; skip because localized to string conversion, it works
      ;; (test-equal "5.1" (assoc-ref dbopts-alist "max_bytes_for_level_multiplier"))
      (test-equal "false" (assoc-ref dbopts-alist "level_compaction_dynamic_level_bytes"))
      (test-equal "11" (assoc-ref dbopts-alist "max_subcompactions"))
      (test-equal "12" (assoc-ref dbopts-alist "max_background_jobs"))
      ))

   (test-group
    "options:rocksdb-options-create + rocksdb-block-based-options-create"
    (let* ([bbopts (rocksdb-block-based-options-create :block-size 1
                                                       :block-size-deviation 2
                                                       :block-restart-interval 3
                                                       :index-block-restart-interval 4
                                                       :metadata-block-size 5
                                                       :partition-filters #t
                                                       :use-delta-encoding #t
                                                       :filter-policy '(ribbon-hybrid 1.23 4.56)
                                                       :no-block-cache #f
                                                       :block-cache (rocksdb-cache-create-lru 65536)
                                                       ;; :cache-compressed (rocksdb-cache-create-lru 65536)
                                                       :whole-key-filtering #f
                                                       :format-version 5
                                                       :index-type 'two-level-index-search
                                                       :data-block-index-type 'binary-search-and-hash
                                                       :data-block-hash-ratio 7.89
                                                       :hash-index-allow-collision #t
                                                       :cache-index-and-filter-blocks #t
                                                       :cache-index-and-filter-blocks-with-high-priority #t
                                                       :pin-l0-filter-and-index-blocks-in-cache #t
                                                       :pin-top-level-index-and-filter #t)]
           [dbopts (rocksdb-options-create :table-factory bbopts :create-if-missing #t)]
           [db (rocksdb-open dbopts (make-tmp-dir))]
           [dbopts-alist (options->alist dbopts)])
      (test-assert (rocksdb-options? dbopts))
      (rocksdb-put db #u8(1 1 1) #u8(2 2 2))
      (test-equal #u8(2 2 2) (rocksdb-get db #u8(1 1 1)))
      (test-assert (not (rocksdb-closed? db)))))

   (test-group
    "options:rocksdb-options-create + rocksdb-cuckoo-options-create"
    (let* ([cuckopts (rocksdb-cuckoo-options-create :hash-ratio 123.321
                                                    :max-search-depth 5
                                                    :cuckoo-block-size 123
                                                    :identity-as-first-hash #t
                                                    :use-module-hash #t)]
           [dbopts (rocksdb-options-create :table-factory cuckopts :create-if-missing #t)]
           [db (rocksdb-open dbopts (make-tmp-dir))]
           [dbopts-alist (options->alist dbopts)])
      (test-assert (rocksdb-options? dbopts))
      (rocksdb-put db #u8(1 1 1) #u8(2 2 2))
      (test-equal #u8(2 2 2) (rocksdb-get db #u8(1 1 1)))
      (test-assert (not (rocksdb-closed? db)))))

   (test-group
    "options:rocksdb-options-create + rocksdb-plain-options-create"
    (let* ([plainopts (rocksdb-plain-options-create 123 16 0.81 10)]
           [dbopts (rocksdb-options-create :table-factory plainopts :create-if-missing #t)]
           [db (rocksdb-open dbopts (make-tmp-dir))]
           [dbopts-alist (options->alist dbopts)])
      (test-assert (rocksdb-options? dbopts))
      (rocksdb-put db #u8(1 1 1) #u8(2 2 2))
      (test-equal #u8(2 2 2) (rocksdb-get db #u8(1 1 1)))
      (test-assert (not (rocksdb-closed? db)))))

   (test-group
    "options:copy/destroy test"
    (let* ([plainopts (rocksdb-plain-options-create 123 16 0.81 10)]
           [cuckopts (rocksdb-cuckoo-options-create :hash-ratio 123.321
                                                    :max-search-depth 98
                                                    :cuckoo-block-size 7
                                                    :identity-as-first-hash #t
                                                    :use-module-hash #f)]
           [bbopts (rocksdb-block-based-options-create :block-size 8192
                                                       :block-size-deviation 20
                                                       :block-restart-interval 3
                                                       :index-block-restart-interval 4
                                                       :metadata-block-size 5
                                                       :partition-filters #t
                                                       :use-delta-encoding #t
                                                       :filter-policy '(bloom-full 12.345)
                                                       :no-block-cache #f
                                                       ;; :block-cache (rocksdb-cache-create-lru 65536)
                                                       :cache-compressed (rocksdb-cache-create-lru 65536)
                                                       :whole-key-filtering #f
                                                       :format-version 5
                                                       :index-type 'two-level-index-search
                                                       :data-block-index-type 'binary-search-and-hash
                                                       :data-block-hash-ratio 7.89
                                                       :hash-index-allow-collision #t
                                                       :cache-index-and-filter-blocks #t
                                                       :cache-index-and-filter-blocks-with-high-priority #t
                                                       :pin-l0-filter-and-index-blocks-in-cache #t
                                                       :pin-top-level-index-and-filter #t)]

           [dbopts-p1 (rocksdb-options-create :table-factory plainopts :create-if-missing #t)]
           [dbopts-p2 (rocksdb-options-create :table-factory plainopts :create-if-missing #t)] ;copy
           [dbopts-c1 (rocksdb-options-create :table-factory cuckopts :create-if-missing #t)]
           [dbopts-c2 (rocksdb-options-create :table-factory cuckopts :create-if-missing #t)] ;copy
           [dbopts-b1 (rocksdb-options-create :table-factory bbopts :create-if-missing #t)]
           [dbopts-b2 (rocksdb-options-create :table-factory bbopts :create-if-missing #t)] ;copy

           [dbopts-unused (rocksdb-options-create :table-factory bbopts :create-if-missing #t)]
           [plainopts-unused (rocksdb-plain-options-create 123 16 0.81 10)]
           [cuckopts-unused (rocksdb-cuckoo-options-create :hash-ratio 123.321)]
           [bbopts-unused (rocksdb-block-based-options-create :block-size 1)])

      (define (with-db fun dbopts)
        (let ([dir (make-tmp-dir)])
          (fun (rocksdb-open dbopts dir) dir)))

      (define (test-config dbopts key specific-tests)
        (let* ([dir (make-tmp-dir)]
               [db (rocksdb-open dbopts dir)])
          (rocksdb-put db #u8(1 1 1) #u8(2 2 2))
          (test-equal #u8(2 2 2) (rocksdb-get db #u8(1 1 1)))
          (test-assert (not (rocksdb-closed? db)))
          (rocksdb-close! db)
          (specific-tests (assoc-ref (option-file->alist dir) key))))

      (define (validate-plain opts)
        (test-equal "123" (assoc-ref opts "user_key_len"))
        (test-equal "10" (assoc-ref opts "index_sparseness"))
        (test-equal "16" (assoc-ref opts "bloom_bits_per_key"))
        (test-equal "0,810000" (assoc-ref opts "hash_table_ratio")))

      (define (validate-cuckoo opts)
        (test-equal "false" (assoc-ref opts "use_module_hash"))
        (test-equal "123,321000" (assoc-ref opts "hash_table_ratio"))
        (test-equal "7" (assoc-ref opts "cuckoo_block_size"))
        (test-equal "98" (assoc-ref opts "max_search_depth"))
        (test-equal "true" (assoc-ref opts "identity_as_first_hash")))

      (define (validate-bb opts)
        (test-equal "true" (assoc-ref opts "pin_top_level_index_and_filter"))
        (test-equal "20" (assoc-ref opts "block_size_deviation"))
        (test-equal "false" (assoc-ref opts "whole_key_filtering"))
        (test-equal "5" (assoc-ref opts "metadata_block_size"))
        (test-equal "5" (assoc-ref opts "format_version"))
        (test-equal "8192" (assoc-ref opts "block_size"))
        (test-equal "true" (assoc-ref opts "cache_index_and_filter_blocks"))
        (test-equal "true" (assoc-ref opts "partition_filters"))
        (test-equal "true" (assoc-ref opts "hash_index_allow_collision"))
        (test-equal "3" (assoc-ref opts "block_restart_interval"))
        (test-equal "true" (assoc-ref opts "cache_index_and_filter_blocks_with_high_priority"))
        (test-equal "rocksdb.BuiltinBloomFilter" (assoc-ref opts "filter_policy")) ;FIXME: ribbon?
        (test-equal "true" (assoc-ref opts "pin_l0_filter_and_index_blocks_in_cache"))
        (test-equal "7,890000" (assoc-ref opts "data_block_hash_table_util_ratio"))
        (test-equal "4" (assoc-ref opts "index_block_restart_interval"))
        (test-equal "false" (assoc-ref opts "no_block_cache"))
        (test-equal "kTwoLevelIndexSearch" (assoc-ref opts "index_type"))
        (test-equal "kDataBlockBinaryAndHash" (assoc-ref opts "data_block_index_type"))
        (test-equal "false" (assoc-ref opts "whole_key_filtering"))
        (test-equal "20" (assoc-ref opts "block_size_deviation")))

      (test-config dbopts-p1 "[TableOptions/PlainTable \"default\"]" validate-plain)
      (test-config dbopts-p2 "[TableOptions/PlainTable \"default\"]" validate-plain)

      (test-config dbopts-c1 "[TableOptions/CuckooTable \"default\"]" validate-cuckoo)
      (test-config dbopts-c2 "[TableOptions/CuckooTable \"default\"]" validate-cuckoo)

      (test-config dbopts-b1 "[TableOptions/BlockBasedTable \"default\"]" validate-bb)
      (test-config dbopts-b2 "[TableOptions/BlockBasedTable \"default\"]" validate-bb)

      ))
   ))
