/* Sort the runnable steps by priority. Priority is establised
   as follows (in order of precedence):

   - The global priority of the builds that depend on the
     step. This allows admins to bump a build to the front of
     the queue.

   - The lowest used scheduling share of the jobsets depending
     on the step.

   - The local priority of the build, as set via the build's
     meta.schedulingPriority field. Note that this is not
     quite correct: the local priority should only be used to
     establish priority between builds in the same jobset, but
     here it's used between steps in different jobsets if they
     happen to have the same lowest used scheduling share. But
     that's not very likely.

   - The lowest ID of the builds depending on the step;
     i.e. older builds take priority over new ones.

   FIXME: O(n lg n); obviously, it would be better to keep a
   runnable queue sorted by priority. */
class StepInfo
{
public:
    Step::ptr step;

    /* The lowest share used of any jobset depending on this
       step. */
    double lowestShareUsed = 1e9;

    /* Info copied from step->state to ensure that the
       comparator is a partial ordering (see MachineInfo). */
    int highestGlobalPriority;
    int highestLocalPriority;
    BuildID lowestBuildID;

        /* Using the TAGS scheduling algorithm
       (Task Assignment with Unknown Duration,
       Mor Harchol-Balter, 2002) we start each build job with
       a low set of resources allocated to it.

       On the first try it gets 30s of build time and 1 core.
       If it takes too long, the job is killed and rescheduled
       with more time and cores.

       On each subsequent try, it is increased. Instead of
       tracking these resources independently, we instead
       track the "rung" the build is on, and calculate those
       granted resources.

       I use the analogy of a "ladder", and climbing rungs of
       the ladder as it progresses up the allocated resource
       count.
    */
    int rung = 1;

    StepInfo(Step::ptr step, Step::State & step_) : step(step)
    {
        for (auto & jobset : step_.jobsets)
            lowestShareUsed = std::min(lowestShareUsed, jobset->shareUsed());
        highestGlobalPriority = step_.highestGlobalPriority;
        highestLocalPriority = step_.highestLocalPriority;
        lowestBuildID = step_.lowestBuildID;
    }


    /*
      Return the amount of time the job should be permitted to run. This
      number could get infinitely large, but the job should be considered
      failed if the permitted run time exceeds Hydra's considered maximum.

      Progression: 30s, 5min, 50min, 8hrs, 3.5 days
    */
    int getPermittedRunTime(void) {
        return 3 * pow(10, rung);
    }

    /*
      Return the number of desired cores. This number is aspirational and
      is limited by the maximum number of cores available on a single
      machine.

      The number of desired cores is bounded by the number of attempts
      which will fit within permitted runtime.

      Given a Hydra maximum of 10 hours and the permitted run time
      function of 3 * (10 ^ rung), then there will be a maximum of 5
      retries, thus a maximum of 5^2 cores.

      Really, though, figuring out a smarter way to do this would be nice.

      Progression: 1, 4, 9, 16, 25
    */
    int getDesiredCores(void) {
        return pow(rung, 2);
    }
};
