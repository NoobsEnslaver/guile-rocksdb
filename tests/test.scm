(load-extension "c_src/.libs/libguile-rocksdb" "init")

(use-modules (rocksdb)
             (rocksdb options)
             (rocksdb table-options)
             (rocksdb cache)
             (rocksdb env)
             (rocksdb writeoptions)
             (rocksdb readoptions)
             (rocksdb iterator)
             (rnrs bytevectors)
             (srfi srfi-1)
             (srfi srfi-64)
             (ice-9 receive)
             (ice-9 rdelim)
             (oop goops))

(read-set! keywords 'prefix)

(define cleanup-strategy 'purge)    ;keep-logs | purge | none
(set! test-log-to-file "guile-rocksdb.log")
(define repeat 10)

(define (test-not-equal a b)
  (test-assert (not (equal? a b))))

(define (make-tmp-dir)
  (mkdtemp "tmp-XXXXXX"))

(define (alist=? a b)
  (every (lambda (x) (equal? x (assoc (car x) b))) a))

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

(define* (with-db fun :optional (dbopts (rocksdb-options-create :create-if-missing #t)))
    (fun (rocksdb-open dbopts (make-tmp-dir))))

(let ([top-dir (getcwd)]
      [test-dir (make-tmp-dir)])
  (dynamic-wind
    (lambda ()
      (chdir test-dir)
      (test-begin "rocksdb-guile"))
    (lambda ()
      (do ([i 1 (+ i 1)]) ((> i repeat))
        (begin (include "test-main.scm"))
        (begin (include "test-options.scm"))
        (begin (include "test-iterator.scm"))
        (begin (include "test-readoptions.scm"))
        (gc)))
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
