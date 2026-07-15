using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TelescopeRTS
{
  // Represents one MS/MS scan waiting to be dispatched to a worker thread, tagged with
  // the high-resolution timestamp (in Stopwatch ticks) at which it was queued so the
  // eventual wait time can be measured.
  public class ScanQueueItem
  {
    public int scanIndex = 0;
    public long queuedAtTicks = 0;
    public ScanQueueItem(int scanIndex, long queuedAtTicks)
    {
      this.scanIndex = scanIndex;
      this.queuedAtTicks = queuedAtTicks;
    }
  }
}
