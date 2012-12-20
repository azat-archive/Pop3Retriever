Description:
===========
Pop3Retriever is a command line tool to backup mail from a POP3 mailserver.  
The program was originally written for 2 reasons. First of all, I needed central backup of mails. And because I read my mail on multiple PC's, I don't want to delete them on the server immediately. (And my client -opera- doesn't support "delete mails older than X days").  

To use the program, create a configfile (sample included) and pass it as an argument to the program.

Features:
========
- store mail in mbox-file(s)
- delete mail from server when older than X days
- group mails by date in single files
- Multiple accounts
- ssl

Changelog:
=========
- v0.9
    - Initial version:
    - store mail in mbox-file(s)
    - delete mail from server when older than X days
    - group mails by date in single files
    - Multiple accounts
- v0.99
    - Add ssl support

Depends on  Qt 4.x
------------------
