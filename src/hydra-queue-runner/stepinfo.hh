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
struct StepInfo
{
    Step::ptr step;

    /* The lowest share used of any jobset depending on this
       step. */
    double lowestShareUsed = 1e9;

    /* Info copied from step->state to ensure that the
       comparator is a partial ordering (see MachineInfo). */
    int highestGlobalPriority;
    int highestLocalPriority;
    BuildID lowestBuildID;

    StepInfo(Step::ptr step, Step::State & step_) : step(step)
    {
        for (auto & jobset : step_.jobsets)
            lowestShareUsed = std::min(lowestShareUsed, jobset->shareUsed());
        highestGlobalPriority = step_.highestGlobalPriority;
        highestLocalPriority = step_.highestLocalPriority;
        lowestBuildID = step_.lowestBuildID;
    }
};
