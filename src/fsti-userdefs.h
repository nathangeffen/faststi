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

#define FSTI_AGENT_PRINT_CSV(file_handle, id, agent)                  \
    fprintf(file_handle, "%u,%zu,%u,%u,%u,%u,%u,%u,%f,%f,%ld\n",     \
            id,                                                       \
            agent->id,                                                \
            (unsigned) agent->sex,                                    \
            (unsigned) agent->sex_preferred,                          \
            (unsigned) agent->birthday,                               \
            (unsigned) agent->age_group,                              \
            agent->infected,                                          \
            agent->hiv,                                               \
            agent->re_sexact,                                         \
            agent->relstat,                                           \
            agent->num_partners ? (long) agent->partners[0] : -1)

#define FSTI_CSV_ENTRIES                                        \
    FSTI_CSV_ENTRY(id, fsti_to_size_t)                          \
    FSTI_CSV_ENTRY(birthday, fsti_to_uint16_t)                   \
    FSTI_CSV_ENTRY(age_group, fsti_to_uint16_t)                 \
    FSTI_CSV_ENTRY(sex, fsti_to_uchar)                          \
    FSTI_CSV_ENTRY(sex_preferred, fsti_to_uchar)                \
    FSTI_CSV_ENTRY(re_sexact, fsti_to_double)                   \
    FSTI_CSV_ENTRY(relstat, fsti_to_double)                     \
    FSTI_CSV_ENTRY(hiv, fsti_to_bool)                           \
    FSTI_CSV_ENTRY(partners[0], fsti_to_partner)

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
#endif
