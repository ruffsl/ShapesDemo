// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/**
 * @file ShapeSubscriber.cpp
 *
 */

#include <eprosimashapesdemo/shapesdemo/ShapeSubscriber.h>
#include <eprosimashapesdemo/qt/ContentFilterSelector.h>
#include <fastrtps/utils/TimeConversion.h>

#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/Domain.h>

ShapeSubscriber::ShapeSubscriber(Participant* par):
    mp_sub(nullptr),
    mp_participant(par),
    hasReceived(false),
    m_mutex(QMutex::Recursive),
    mp_contentFilter(nullptr)
{
    // TODO Auto-generated constructor stub

}

ShapeSubscriber::~ShapeSubscriber() {
    // TODO Auto-generated destructor stub
    Domain::removeSubscriber(mp_sub);
    if(mp_contentFilter!=nullptr)
        delete(mp_contentFilter);
}

bool ShapeSubscriber::initSubscriber()
{
    mp_sub = Domain::createSubscriber(mp_participant,m_attributes,(SubscriberListener*)this);
    if(mp_sub !=nullptr)
        return true;
    return false;
}




void ShapeSubscriber::onNewDataMessage(Subscriber* sub)
{
    // cout << "New DATA Message "<<endl;
    Shape shape;
    shape.m_type = this->m_shapeType;
    SampleInfo_t info;
    while(sub->takeNextData((void*)&shape.m_shape,&info))
    {
        // shape.m_x += 5;
        //cout << "Shape of type: "<< shape.m_type << "RECEIVED"<<endl;
        shape.m_time = info.sourceTimestamp;
        shape.m_writerGuid = info.sample_identity.writer_guid();
        shape.m_strength = info.ownershipStrength;
        QMutexLocker locck(&this->m_mutex);
        if(info.sampleKind == ALIVE)
        {
            hasReceived = true;
            m_shapeHistory.addToHistory(shape);
        }
        else
        {
            //cout << "NOT ALIVE DATA"<<endl;
            //GET THE COLOR:
            SD_COLOR color = getColorFromInstanceHandle(info.iHandle);
            if(info.sampleKind == NOT_ALIVE_DISPOSED)
            {
                m_shapeHistory.dispose(color);
            }
            else
            {
                m_shapeHistory.unregister(color);
            }
        }
    }
}



void ShapeSubscriber::onSubscriptionMatched(Subscriber* /*sub*/, MatchingInfo& info)
{
    if(info.status ==MATCHED_MATCHING)
    {
        //cout << "Subscriber in topic " << m_attributes.topic.getTopicName() << " MATCHES Pub: " << info.remoteEndpointGuid <<"*****************************"<<endl;
        bool found = false;
        for(std::vector<GUID_t>::iterator it = m_remoteWriters.begin();
            it!=m_remoteWriters.end();++it)
        {
            if(*it==info.remoteEndpointGuid)
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            m_remoteWriters.push_back(info.remoteEndpointGuid);
        }

    }
    else if(info.status == REMOVED_MATCHING)
    {
        //cout << "Subscriber in topic " << m_attributes.topic.getTopicName() << " REMOVES Pub: " << info.remoteEndpointGuid <<"*****************************"<<endl;
        m_mutex.lock();
        m_shapeHistory.removedOwner(info.remoteEndpointGuid);
        m_mutex.unlock();
    }
}

void ShapeSubscriber::adjustContentFilter(ShapeFilter &filter)
{
    m_mutex.lock();
    m_shapeHistory.adjustContentFilter(filter);
    m_mutex.unlock();
}

//void ShapeSubscriber::removeSamplesFromWriter(GUID_t)

