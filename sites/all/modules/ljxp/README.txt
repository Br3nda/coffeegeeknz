ljxp version 6.x-1.0 (official D6 release)

This module is modelled after the WordPress plugin of the same name; I'd call
it a port, but there are only a few sections of code that actually
remain the same. So while it's not actually technically original, it's
about 80% different. That said, I must give credit to the original author
who did a lot of work in getting the WordPress plugin working:

http://ebroder.net/livejournal-crossposter/

To install:

1) drop the entire directory into your modules or sites/*/modules directory
2) go to administer >> modules and enable the module
3) go to administer >> settings >> ljxp and enable the content types you want
   to use
4) If you want more than just UID #1 to use this functionality, Go to 
   administer >> access control and set the 'can crosspost to livejournal'
   permission for a role.
5) go to 'my account' and choose edit; enter your LiveJournal info there.
   Each user must have his or her own crossposting information.

TODO:

o A way to automatically post old entries.
o Rigorous testing.
o A 'test' button on the my account that simply logs in and ensures that
  the data is correct.
o A little more flexibility with the header, perhaps.