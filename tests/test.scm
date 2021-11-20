(load-extension "c_src/.libs/libguile-rocksdb" "init")

(use-modules (rocksdb)
             (rocksdb options)
             (rocksdb cache)
             (rocksdb env)
             (rocksdb writeoptions)
             (rocksdb readoptions)
             (rnrs bytevectors)
             (srfi srfi-64)
             (ice-9 receive))

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

(let ([top-dir (getcwd)]
      [test-dir (make-tmp-dir)])
  (dynamic-wind
    (lambda ()
      (chdir test-dir)
      (test-begin "rocksdb-guile"))
    (lambda ()
      (include "test-main.scm"))
    (lambda ()
      (test-end)
      (chdir top-dir)
      (case cleanup-strategy
        ((purge)
         (rm-rf test-dir))
        ((keep-logs)
         (rename-file (string-append "./" test-dir "/rocksdb-guile.log") "./rocksdb-guile.log")
         (rm-rf test-dir)))
      )))


;; (if (errors)
;;     (exit EXIT_FAILURE)
;;     (exit EXIT_SUCCESS))
