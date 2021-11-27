(load-extension "c_src/.libs/libguile-rocksdb" "init")

(use-modules (rocksdb)
             (rocksdb options)
             (rocksdb cache)
             (rocksdb env)
             (rocksdb writeoptions)
             (rocksdb readoptions)
             (rocksdb iterator)
             (rnrs bytevectors)
             (srfi srfi-1)
             (srfi srfi-64)
             (ice-9 receive)
             (oop goops))

(define cleanup-strategy 'purge)    ;keep-logs | purge | none

(define (test-not-equal a b)
  (test-assert (not (equal? a b))))

(define (make-tmp-dir)
  (mkdtemp "tmp-XXXXXX"))

(define (rm-rf tree)
  (cond
   ((file-is-directory? tree)
    (let ((dir (opendir tree)))
      (let loop ()
        (let ((entry (readdir dir)))
          (cond
           ((member entry '("." ".."))
            (loop))
           ((not (eof-object? entry))
            (let ((name (string-append tree "/" entry)))
              (rm-rf name)
              (loop))))))
      (closedir dir)
      (rmdir tree)))
   ((file-exists? tree)
    (delete-file tree))
   (else
    (error "rm-rf: can't delete " tree))))

(define (with-db fun)
  (let ([dbopts (rocksdb-options-create)])
    (rocksdb-options-set-create-if-missing! dbopts 1)
    (fun (rocksdb-open dbopts (make-tmp-dir)))))

(set! test-log-to-file "guile-rocksdb.log")
(let ([top-dir (getcwd)]
      [test-dir (make-tmp-dir)])
  (dynamic-wind
    (lambda ()
      (chdir test-dir)
      (test-begin "rocksdb-guile"))
    (lambda ()
      (include "test-main.scm")
      (include "test-options.scm")
      (include "test-iterator.scm"))
    (lambda ()
      (test-end)
      (chdir top-dir)
      (case cleanup-strategy
        ((purge)
         (rm-rf test-dir))
        ((keep-logs)
         (rename-file (string-append "./" test-dir "/" test-log-to-file)
                      (string-append "./" test-log-to-file))
         (rm-rf test-dir)))
      )))


;; (if (errors)
;;     (exit EXIT_FAILURE)
;;     (exit EXIT_SUCCESS))
