// TO COMPILE : gcc -Wall ksu.c -lpam -lpam_misc -o ksu
/* TODO
* sudo chown root ksu
* sudo chgrp root ksu
* sudo chmod 4711 ksu
*/

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

// Pour converser entrer l'utilisateur et le programme
static struct pam_conv conv = {
    misc_conv,
    NULL
};
struct passwd *pwd;

int ksu(int argc, char *argv[])
{
    pam_handle_t *pamh = NULL;
    int retval;
    const char *user = "root"; // utilisateur par défaut (si aucun argument n'est entrée)

    if(argc > 2) {
        fprintf(stderr, "ksu : too many arguments\n"); // A REVOIR car su peut à priori en prendre plusieurs
        exit(EXIT_FAILURE);
    }

    if(argc == 2) {
        user = argv[1]; 
    }

    retval = pam_start("check_user", user, &conv, &pamh);

    if (retval != PAM_SUCCESS)
        fprintf(stderr, "Error with pam_start\n");

    if (retval == PAM_SUCCESS) // authentification
        retval = pam_authenticate(pamh, 0);    

    if (retval == PAM_SUCCESS)
        retval = pam_acct_mgmt(pamh, 0);       

    if (retval != PAM_SUCCESS) { // pas le bon mdp donc on quitte
        fprintf(stderr, "ksu: Authentication failure\n");
        exit(EXIT_FAILURE);
    } 

    retval = pam_setcred(pamh, 0);
    if (retval != PAM_SUCCESS)
        fprintf(stderr, "Error with user credentials\n");

    retval = pam_open_session(pamh, 0);
    if (retval != PAM_SUCCESS)
        fprintf(stderr, "Error with openning session\n");

    pwd = getpwnam(user);
    if( setuid(pwd->pw_uid) != 0 ) 
        fprintf(stderr, "Error with setting user id\n");

    if (pam_end(pamh,retval) != PAM_SUCCESS) {     /* close Linux-PAM */
        pamh = NULL;
        fprintf(stderr, "check_user: failed to release authenticator\n");
        exit(EXIT_FAILURE);
    }

    // on lance un nouveau bash avec le nouveau user
    if (retval == PAM_SUCCESS)
        execvp("bash", (char*[]){"bash", NULL});

    return retval;
}