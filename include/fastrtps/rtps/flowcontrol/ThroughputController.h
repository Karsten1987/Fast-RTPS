// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THROUGHPUT_CONTROLLER_H
#define THROUGHPUT_CONTROLLER_H

#include <fastrtps/rtps/flowcontrol/FlowController.h>
#include <thread>

namespace eprosima{
namespace fastrtps{
namespace rtps{

/**
 * Descriptor for a Throughput Controller, containing all constructor information
 * for it. 
 * @ingroup NETWORK_MODULE
 */
struct ThroughputControllerDescriptor {
   //! Packet size in bytes that this controller will allow in a given period.
   uint32_t bytesPerPeriod;
   //! Window of time in which no more than 'bytesPerPeriod' bytes are allowed.
   uint32_t periodMillisecs;
   RTPS_DllAPI ThroughputControllerDescriptor();
   RTPS_DllAPI ThroughputControllerDescriptor(uint32_t size, uint32_t time);
};

class RTPSWriter;
class RTPSParticipantImpl;

/**
 * Simple filter that only clears changes up to a certain accumulated payload size.
 * It refreshes after a given time in MS, in a staggered way (e.g. if it clears
 * 500kb at t=0 and 800 kb at t=10, it will refresh 500kb at t = 0 + period, and
 * then fully refresh at t = 10 + period).
 */
class ThroughputController : public FlowController
{
public:
   ThroughputController(const ThroughputControllerDescriptor&, const RTPSWriter* associatedWriter);
   ThroughputController(const ThroughputControllerDescriptor&, const RTPSParticipantImpl* associatedParticipant);
   virtual void operator()(std::vector<CacheChangeForGroup_t>& changes);

private:
   uint32_t mBytesPerPeriod;
   uint32_t mAccumulatedPayloadSize;
   uint32_t mPeriodMillisecs;
   std::recursive_mutex mThroughputControllerMutex;

   const RTPSParticipantImpl* mAssociatedParticipant;
   const RTPSWriter* mAssociatedWriter;

   /*
    * Schedules the filter to be refreshed in period ms. When it does, its capacity
    * will be partially restored, by "sizeToRestore" bytes.
    */
   void ScheduleRefresh(uint32_t sizeToRestore);
};

} // namespace rtps
} // namespace fastrtps
} // namespace eprosima

#endif
