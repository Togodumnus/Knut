#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> //remove

#include "lectureAction.h"
#include "Action.h"
#include "Command.h"
#include "../DEBUG.h"

/**
 * removeCharFromString
 *
 * Va supprimer le caractère ch de str en décallant la suite de str
 *
 * @param  {char *} str     La chaîne
 * @param  {char *} ch      Un caractère de la str
 */
void removeCharFromString(char *str, char *ch) {
    char copy[strlen(str)];
    strcpy(copy, str);
    *ch = '\0';
    strcat(str, copy + (ch - str) + 1);
}

/**
 * createString
 *
 * Copie un morceau d'une chaîne de charactère après avoir alloué l'espace
 * nécessaire
 *
 * @param  {char *}     str     Le pointeur sur le premier caractère du message
 * @param  {int}        length  La longueur du message
 * @return {char *}             Un pointeur vers le message. À free() après.
 */
char *createString(char *str, int length) {

    char *string = (char *) malloc((length + 1) * sizeof(char));
    DEBUG("Malloc string = %p, %d", string, getpid());
    if (string == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }

    strncpy(string, str, length + 1);
    string[length] = '\0';

    return string;
}

/**
 * addArg
 *
 * Ajout d'un argument à une Command
 *
 * @param  {Command *}  cmd     Un pointeur sur la Command à compléter
 * @param  {char *}     arg     Un pointeur sur la chaîne
 */
void addArg(Command *cmd, char *arg) {

    DEBUG("addArg");

    cmd->argc++; //on MAJ le nombre d'arguments

    //on agrandit le tableau d'arguments comme il faut bien
    cmd->argv = (char **) realloc(cmd->argv, (cmd->argc + 1) * sizeof(char *));
    if (cmd->argv == NULL) {
        perror("Realloc error");
        exit(EXIT_FAILURE);
    }

    cmd->argv[cmd->argc - 1] = arg;
}

/**
 * completeCmdAndArg
 *
 * Complète la Command avec une chaîne de caractère qui est soit le nom de la
 * commande, soit un argument
 *
 * @param  {Command *}  cmd     La commande à compléter
 * @param  {char *}     start   Un pointeur sur la chaîne
 * @param  {int}        length  La taille du message à extraire de str
 */
void completeCmdAndArg(Command *cmd, char *start, int length) {

    DEBUG("complete cmd or arg");
    DEBUG("*start = %c", *start);

    if (length > 0 && *start != ' ') { //on n'ajoute pas de chaîne vide
        char *string = createString(start, length);
        addArg(cmd, string);
        if (cmd->cmd == NULL) { //premier argument = le nom de la command
            cmd->cmd = string;
        }
    }
}

/**
 * lectureAction
 *
 * Automate de lecture d'une action
 *
 * @param  {Action *}   action  L'action à lire
 * @return {Command *}          Une nouvelle commande à exécuter
 *                              (doit être free()) plus loin
 */
Command *lectureAction(Action *action) {

    DEBUG("Début Lecture action");

    /**
     * start
     *
     * Pointeur sur le mot courant
     */
    char *start = action->cmd;

    /**
     * length
     *
     * Taille du mot courant
     */
    int length = -1;

    /*
     * Les fichiers de redirections
     */

    bool toFile   = false;
    bool fromFile = false;
    bool appendFile = false;

    /**
     * state
     *
     * États de l'automate
     */
    enum LECTURE_STATES state = START;

    /**
     * cmd
     *
     * La Command à compléter
     */
    Command *cmd = (Command *) malloc(sizeof(Command));
    memset(cmd, 0, sizeof(Command)); //init à 0
    cmd->argv = (char **) malloc(sizeof(char *));
    DEBUG("cmd->argv %p", cmd->argv);
    if (cmd == NULL || cmd->argv == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }

    /**
     * ch
     *
     * Le charactère courant
     */
    char ch;

    /**
     * withSpace
     *
     * True si on est passé sur un \ servant à un argument avec espace.
     * @Example     cat Mon\ Super \Fichier
     */
    bool withSpace = false;

    bool done = false;

    do {

        length++;
        ch = *(start + length);

        DEBUG("current char : %c, current length : %d",
                ch, length);

        switch (state) {

            case START:
                DEBUG("START %c", ch);
                if (ch == ' ') { //trim espace
                    start += 1; //RAZ
                    length -= 1;
                } else if (ch == '(') {
                    //la commande est une sous-action
                    state = READING_COMPLEX;
                    cmd->type = COMPLEX;
                    start += 1; //RAZ
                    length = -1;
                } else {
                    state = READING_COMMAND;
                }
                break;

            case READING_COMPLEX:
                DEBUG("READING_COMPLEX %c", ch);
                if (length == 0 &&  ch == ' ') { //premiers espaces
                    start += 1; //RAZ
                    length = -1;
                } else if (ch == ')') {
                    done = true;
                }

                break;

            case READING_COMMAND:
                DEBUG("READING_COMMAND %c", ch);
                if (ch == '\\') {
                    DEBUG("READ \\");
                    withSpace = true;
                    removeCharFromString(action->cmd, start + length);
                    length -= 1;
                } else if (ch == '<' || ch == '>' || ch == ' ') {
                    if (ch == '<') {
                        fromFile = true;
                        state = CHEVRON;
                    } else if (ch == '>') {
                        toFile = true;
                        state = CHEVRON;
                    }

                    if (ch == ' ' && withSpace) {
                        withSpace = false;
                        continue;
                    }
                    //on enregistre le mot précédent
                    completeCmdAndArg(cmd, start, length);
                    start += length + 1; //RAZ
                    length = -1;
                }
                //else on continue à lire
                break;

            case CHEVRON:
                DEBUG("CHEVRON %c", ch);
                state = READING_FILE;
                if (ch == '>' || ch == '<') {
                    appendFile = true;
                    start += 1;
                }
                length = -1;

                break;

            case READING_FILE:
                DEBUG("READING_FILE %c", ch);
                if (ch == '\\') {
                    DEBUG("READ \\");
                    withSpace = true;
                    removeCharFromString(action->cmd, start + length);
                    length -=1;
                } else if (ch == ' ' && length == 0) { //espace au début
                    //ne pas retenir cet espace
                    start += 1;
                } else if (ch == ' ' && length > 0 && !withSpace) {
                    //espace à la fin
                    done = true;
                }

                if (ch == ' ' && withSpace) {
                    withSpace = false;
                    continue;
                }

                break;

            default:
                fprintf(stderr, "Can't read %s", action->cmd);
                exit(EXIT_FAILURE);
                break;

        }

    } while (!done && ch != '\n' && ch != '\0');

    //On complète la Command
    if (toFile) {
        cmd->toFile = createString(start, length);
        cmd->appendFile = appendFile;
    } else if (fromFile) {
        cmd->fromFile = createString(start, length);
        cmd->appendFile = appendFile;
    } else {
        completeCmdAndArg(cmd, start, length);
    }
    cmd->argv[cmd->argc] = NULL;

    //débug
    DEBUG("Action  %s", action->cmd);
    DEBUG("Command %s", cmd->cmd);
    DEBUG("Command type %d", cmd->type);
    DEBUG("Arg : ");
    for (int i = 0; i <= cmd->argc; i++) {
        DEBUG("\t- %s", cmd->argv[i]);
    }
    DEBUG("toFile: %s", cmd->toFile);
    DEBUG("fromFile: %s", cmd->fromFile);
    DEBUG("appendFile: %s", cmd->appendFile == true ? "true" : "false");

    return cmd;
}

/**
 * freeCommand
 *
 * @param  {Command *}  cmd
 */
void freeCommand(Command *cmd) {

    //le tableau d'argument
    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    free(cmd->argv);

    //la commande
    //free(cmd->cmd); //pas besoin car déjà fait par free(argv[0])

    free(cmd);
}

