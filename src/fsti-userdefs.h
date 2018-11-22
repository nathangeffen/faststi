/*
 * Users who wish to extend FastSTI code are encouraged to modify this file and
 * fsti-userdefs.c.
 * It will not modified in FastSTI upgrades.
 *
 */

#ifndef FSTI_USERDEFS // Don't remove these guards
#define FSTI_USERDEFS

#define FSTI_AGENT_FIELDS                                       \
    double re_sexact;                                           \
    double relstat;                                             \
    double reldur;                                              \
    bool hiv;

#define FSTI_HOOK_AFTER_MATCH(simulation, a, b) hestia(simulation, a, b)

#define FSTI_AGENT_PRINT_CSV_HEADER(file_handle, delim)                 \
    fprintf(file_handle, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s\n", \
            "sim", delim,                                               \
            "date", delim,                                              \
            "id", delim,                                                \
            "sex", delim,                                               \
            "sex_preferred", delim,                                     \
            "birthday", delim,                                          \
            "age_group", delim,                                         \
            "death_date", delim,                                        \
            "infected", delim,                                          \
            "hiv", delim,                                               \
            "re_sexact", delim,                                         \
            "relstat", delim,                                           \
            "partner")


#define FSTI_AGENT_PRINT_CSV(file_handle, sim_num, date, agent, delim)  \
    fprintf(file_handle,                                                \
            "%u%c%f%c%zu%c%u%c%u%c%u%c%u%c%f%c%u%c%u%c%f%c%f%c%ld\n",   \
            sim_num,  delim,                                            \
            date, delim,                                                \
            agent->id, delim,                                           \
            (unsigned) agent->sex, delim,                               \
            (unsigned) agent->sex_preferred, delim,                     \
            (unsigned) agent->birthday, delim,                          \
            (unsigned) agent->age_group, delim,                         \
            agent->date_death, delim,                                   \
            agent->infected, delim,                                     \
            agent->hiv, delim,                                          \
            agent->re_sexact, delim,                                    \
            agent->relstat, delim,                                      \
            agent->num_partners ? (long) agent->partners[0] : -1)

#define FSTI_FLEX_REPORT do {                                           \
        FSTI_REPORT_OUTPUT(FSTI_MEAN, living, age_group, "MEAN_AGE_ALIVE");   \
        FSTI_REPORT_OUTPUT(FSTI_MEAN, living, infected, "INFECT_RATE_ALIVE"); \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIZE, living, , "POP_ALIVE", "%.0f"); \
        FSTI_REPORT_OUTPUT_POST_PREC(FSTI_SUM, living, num_partners,   \
                                      "NUM_PARTNERS", FSTI_HALF, "%.0f"); \
        FSTI_REPORT_OUTPUT(FSTI_MEAN, dead, age_group, "MEAN_AGE_DEAD");      \
        FSTI_REPORT_OUTPUT(FSTI_MEAN, dead, infected, "INFECT_RATE_DEAD"); \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIZE, dead, , "POP_DEAD", "%.0f");        \
    } while(0)


#define FSTI_HOOK_EVENTS_REGISTER                       \
    fsti_register_add("AGE", event_age);                \
    fsti_register_add("HADES", event_hades);            \
    fsti_register_add("EROS", event_eros);              \
    fsti_register_add("DIONYSUS", event_dionysus);      \
    fsti_register_add("APHRODITE", event_aphrodite);

// Prototypes
void event_age(struct fsti_simulation *simulation);
void event_hades(struct fsti_simulation *simulation);
void event_eros(struct fsti_simulation *simulation);
void event_dionysus(struct fsti_simulation *simulation);
void event_aphrodite(struct fsti_simulation *simulation);
void hestia(struct fsti_simulation *simulation,
            struct fsti_agent *a, struct fsti_agent *b);


#define FSTI_AGENT_ELEM {                                               \
        FSTI_AGENT_ELEM_ENTRY(age),                                     \
        FSTI_AGENT_ELEM_ENTRY(age_group),                               \
        FSTI_AGENT_ELEM_ENTRY(birth_date),                              \
        FSTI_AGENT_ELEM_ENTRY(birthday),                                \
        FSTI_AGENT_ELEM_ENTRY(cause_of_death),                          \
        FSTI_AGENT_ELEM_ENTRY(cured),                                   \
        FSTI_AGENT_ELEM_ENTRY(date_death),                              \
        FSTI_AGENT_ELEM_ENTRY(hiv),                                     \
        FSTI_AGENT_ELEM_ENTRY(id),                                      \
        FSTI_AGENT_ELEM_ENTRY(infected),                                \
        FSTI_AGENT_ELEM_ENTRY(infected_date),                           \
        FSTI_AGENT_ELEM_ENTRY(num_partners),                            \
        FSTI_AGENT_ELEM_ENTRY(orientation),                             \
        {"partners_0", offsetof(struct fsti_agent, partners),           \
         UINT, fsti_to_partner, NULL},                                  \
        {"partners_1", offsetof(struct fsti_agent, partners[1]),        \
         UINT, NULL, NULL},                                             \
        {"partners_2", offsetof(struct fsti_agent, partners[2]),        \
         UINT, NULL, NULL},                                             \
        {"partners_3", offsetof(struct fsti_agent, partners[3]),        \
         UINT, NULL, NULL},                                             \
        FSTI_AGENT_ELEM_ENTRY(re_sexact),                               \
        FSTI_AGENT_ELEM_ENTRY(relstat),                                 \
        FSTI_AGENT_ELEM_ENTRY(sex),                                     \
        FSTI_AGENT_ELEM_ENTRY(sex_preferred)                            \
    }

#endif
