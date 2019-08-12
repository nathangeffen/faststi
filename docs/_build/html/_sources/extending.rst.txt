
.. _extending_ref:

###################
Extending |PROJECT|
###################

|PROJECT| provides for a wide variety of models without any code being modified
by the user. But some modellers are likely to encounter limitations. For
example, they may want to model TB coinfection with HIV, or implement a cure
event, or more sophisticated agent generation, or provide additional statistics.

To do this, you need to program in C. This guide assumes knowledge of C
programming as well as the source code control system Git.

We recommend the following approach to developing FastSTI.

- Clone the latest version of FastSTI from the `Github repository <https://github.com/nathangeffen/faststi>`_.

- Use either the gcc or Clang C compiler.

- Spend time looking at the source code in the src directory to understand it
  better. In particular we recommend examining the following files:

  - fsti-events.c which contains the events provided by FastSTI.
  - fsti-defaults.c where all the parameters are defined.
  - fsti-defaults.h where many of the macros that can be extended or redefined
    are defined. Note the Hooks section. These are macros that you can define in
    fsti-userdefs.h (or a file included by fsti-userdefs.h) to extend existing
    events and other functionality.
  - fsti-defs.h where some system wide constants and structs are defined.
  - fsti-agent.h where the fsti_agent struct is defined.
  - fsti-simulation.h where the fsti_simulation struct is defined.

Let's say you want to write an event to coinfect agents with TB. This is what
you would need to do:

- In fsti-userdefs.c write a function to do the coinfection. For example:

  .. code-block:: C
     :linenos:

        void tb_coinfect(struct fsti_simulation *simulation)
        {
            struct fsti_agent *agent;
            // Iterate through the living agents
            FSTI_FOR_LIVING(*simulation, agent, {
                // Put the event logic here
            });
        }

- Put the prototype declaration for your event in fsti-userdefs.h. E.g.

  .. code-block:: C
     :linenos:

      void tb_coinfect(struct fsti_simulation *simulation)

- In the fsti-userdefs.h file you need to define the FSTI_HOOK_EVENTS_REGISTER
  macro. This is so that your event can be invoked via the before_events,
  during_events or after_events parameters in the input configuration file.

  .. code-block:: C
     :linenos:

     #define FSTI_HOOK_EVENTS_REGISTER fsti_register_add("tb_coinfect", tb_coinfect);

- Your event may need new parameters and datasets. It may also need new fields
  in the fsti_simulation struct and new fields in the fsti_agent struct.

  - To add new agent fields define them in the FSTI_AGENT_FIELDS macro in
    fsti-userdefs.h (or a file included by fsti-userdefs.h).

  - To add new simulation fields define them in the FSTI_SIMULATION_FIELDS macro
    in fsti-userdefs.h.

  - To provide additional parameters (or datasets) for input configuration files
    redefine the FSTI_ADDITIONAL_CONFIG_VARS parameter in fsti-userdefs.h. An
    example of how to do this is already in the fsti-defaults.h file.

    .. code-block:: C
       :linenos:

       #define FSTI_ADDITIONAL_CONFIG_VARS(config)             \
             FSTI_CONFIG_ADD(config, example_1,                \
             "Example configuration field", 0);                \
             FSTI_CONFIG_ADD_STR(config, dataset_example_2,    \
                        "Example dataset", FSTI_NO_OP)

    Note that the name of a dataset parameter must start *dataset\_*.

  - If you want to save parameters or datasets into the current simulation
    redefine the FSTI_HOOK_CONFIG_TO_VARS macro in fsti-userdefs.h. E.g.

    .. code-block:: C
       :linenos:

          #define FSTI_HOOK_CONFIG_TO_VARS(simulation)               \
                  simulation->example_1 = fsti_connfig_at0_long(     \
                                              &simulation->config,   \
                                              "example_1")
