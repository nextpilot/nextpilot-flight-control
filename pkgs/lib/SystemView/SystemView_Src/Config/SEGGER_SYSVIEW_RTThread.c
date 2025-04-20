/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2015 - 2016  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* * This software may in its unmodified form be freely redistributed *
*   in source form.                                                  *
* * The source code may be modified, provided the source code        *
*   retains the above copyright notice, this list of conditions and  *
*   the following disclaimer.                                        *
* * Modified versions of this software in source or linkable form    *
*   may not be distributed without prior consent of SEGGER.          *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND     *
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A        *
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL               *
* SEGGER Microcontroller BE LIABLE FOR ANY DIRECT, INDIRECT,         *
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES           *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS    *
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS            *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,       *
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING          *
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: V2.40                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_RTThread.c
Purpose : Interface between RT-Thread and System View.
Revision: $Rev: 3745 $
*/

#include "rtthread.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"

#if !((RT_VERSION * 100 + RT_SUBVERSION * 10 + RT_REVISION) > 310L)
    #error "This version SystemView only supports above 3.1.0 of RT-Thread, please select a lower version SystemView!"
#endif

#ifndef PKG_USING_SYSTEMVIEW
    #error "SystemView is only works when feature PKG_USING_SYSTEMVIEW is enable."
#endif
static rt_thread_t tidle;

static U64 _cbGetTime(void)
{
    return (U64)(rt_tick_get() * 1000 / RT_TICK_PER_SECOND);
}

static void _cb_timer_enter(rt_timer_t t)
{
    SEGGER_SYSVIEW_RecordEnterTimer((rt_uint32_t)t);
}

static void _cb_timer_exit(rt_timer_t t)
{
    SEGGER_SYSVIEW_RecordExitTimer();
}

static void _cbSendTaskInfo(const rt_thread_t thread)
{
    SEGGER_SYSVIEW_TASKINFO Info;

    rt_enter_critical();
    rt_memset(&Info, 0, sizeof(Info));
    Info.TaskID = (U32)thread;

    Info.sName = thread->name;
    Info.Prio = thread->current_priority;
    Info.StackBase = (U32)thread->stack_addr;
    Info.StackSize = thread->stack_size;

    SEGGER_SYSVIEW_SendTaskInfo(&Info);
    rt_exit_critical();
}

static void _cbSendTaskList(void)
{
    struct rt_thread *thread;
    struct rt_list_node *node;
    struct rt_list_node *list;
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Thread);
    list = &info->object_list;

    tidle = rt_thread_idle_gethandler();

    rt_enter_critical();
    for (node = list->next; node != list; node = node->next)
    {
        thread = rt_list_entry(node, struct rt_thread, list);
        /* skip idle thread */
        if (thread != tidle)
            _cbSendTaskInfo(thread);
    }
    rt_exit_critical();
}

static void _cb_thread_resume(rt_thread_t thread)
{
    SEGGER_SYSVIEW_OnTaskStartReady((unsigned)thread);
}

static void _cb_thread_suspend(rt_thread_t thread)
{
    SEGGER_SYSVIEW_OnTaskStopReady((unsigned)thread, 0);
}

static void _cb_scheduler(rt_thread_t from, rt_thread_t to)
{
    SEGGER_SYSVIEW_OnTaskStopReady((unsigned)from, 0);
    if (to == tidle)
        SEGGER_SYSVIEW_OnIdle();
    else
    {
        if (rt_interrupt_get_nest())
        {
            SEGGER_SYSVIEW_OnTaskStartReady((unsigned)to);
            SEGGER_SYSVIEW_RecordEnterISR();
        }
        else
            SEGGER_SYSVIEW_OnTaskStartExec((unsigned)to);
    }
}

static void _cb_irq_enter(void)
{
    SEGGER_SYSVIEW_RecordEnterISR();
}

static void _cb_irq_leave(void)
{
    rt_thread_t current;
    if (rt_interrupt_get_nest())
    {
        SEGGER_SYSVIEW_RecordExitISR();
        return;
    }

    SEGGER_SYSVIEW_RecordExitISRToScheduler();
    current = rt_thread_self();
    if (current == tidle)
        SEGGER_SYSVIEW_OnIdle();
    else
        SEGGER_SYSVIEW_OnTaskStartExec((unsigned)current);
}

static void _cb_thread_inited(rt_thread_t thread)
{
    SEGGER_SYSVIEW_OnTaskCreate((rt_uint32_t)thread);
    _cbSendTaskInfo((rt_thread_t)thread);
}

//static void _cb_object_attach(struct rt_object* object)
//{
//    switch(object->type & (~RT_Object_Class_Static))
//    {
//    case RT_Object_Class_Thread:
//        SEGGER_SYSVIEW_OnTaskCreate((unsigned)object);
//        _cbSendTaskInfo((rt_thread_t)object);
//        break;
//    default:
//        break;
//    }
//}

static void _cb_object_detach(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static))
    {
    case RT_Object_Class_Thread:
        SEGGER_SYSVIEW_OnTaskTerminate((unsigned)object);
        break;
    default:
        break;
    }
}

void SEGGER_SYSVIEW_RecordObject(unsigned EventID, struct rt_object *object)
{
    U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32];
    U8 *pPayload;

    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);                      // Prepare the packet for SystemView
    pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, object->name, RT_NAME_MAX);    // Add object name

    if ((object->type & (~RT_Object_Class_Static)) == RT_Object_Class_Event)
        pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, ((rt_event_t)object)->set);

    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, EventID);              // Send the packet
}

static void _cb_object_trytake(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static))
    {
    case RT_Object_Class_Semaphore:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_SEM_TRYTAKE, object);
        break;
    case RT_Object_Class_Mutex:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_MUTEX_TRYTAKE, object);
        break;
    case RT_Object_Class_Event:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_EVENT_TRYTAKE, object);
        break;
    case RT_Object_Class_MailBox:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_MAILBOX_TRYTAKE, object);
        break;
    case RT_Object_Class_MessageQueue:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_QUEUE_TRYTAKE, object);
        break;
    }
}

static void _cb_object_take(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static))
    {
    case RT_Object_Class_Semaphore:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_SEM_TAKEN, object);
        break;
    case RT_Object_Class_Mutex:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_MUTEX_TAKEN, object);
        break;
    case RT_Object_Class_Event:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_EVENT_TAKEN, object);
        break;
    case RT_Object_Class_MailBox:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_MAILBOX_TAKEN, object);
        break;
    case RT_Object_Class_MessageQueue:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_QUEUE_TAKEN, object);
        break;
    }
}

static void _cb_object_put(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static))
    {
    case RT_Object_Class_Semaphore:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_SEM_RELEASE, object);
        break;
    case RT_Object_Class_Mutex:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_MUTEX_RELEASE, object);
        break;
    case RT_Object_Class_Event:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_EVENT_RELEASE, object);
        break;
    case RT_Object_Class_MailBox:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_MAILBOX_RELEASE, object);
        break;
    case RT_Object_Class_MessageQueue:
        SEGGER_SYSVIEW_RecordObject(RTT_TRACE_ID_QUEUE_RELEASE, object);
        break;
    }
}

// Services provided to SYSVIEW by RT-Thread
const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI =
{
    _cbGetTime, _cbSendTaskList,
};

// RT-Thread init trace component
static int rt_trace_init(void)
{
    tidle = rt_thread_idle_gethandler();

    SEGGER_SYSVIEW_Conf();

    // register hooks
    //rt_object_attach_sethook(_cb_object_attach);
    rt_object_detach_sethook(_cb_object_detach);
    rt_object_trytake_sethook(_cb_object_trytake);
    rt_object_take_sethook(_cb_object_take);
    rt_object_put_sethook(_cb_object_put);

    rt_thread_suspend_sethook(_cb_thread_suspend);
    rt_thread_resume_sethook(_cb_thread_resume);
    rt_thread_inited_sethook(_cb_thread_inited);
    rt_scheduler_sethook(_cb_scheduler);

    rt_timer_enter_sethook(_cb_timer_enter);
    rt_timer_exit_sethook(_cb_timer_exit);

    rt_interrupt_enter_sethook(_cb_irq_enter);
    rt_interrupt_leave_sethook(_cb_irq_leave);

    rt_kprintf("RTT Control Block Detection Address is 0x%x\n", &_SEGGER_RTT);

    return 0;
}
INIT_COMPONENT_EXPORT(rt_trace_init);

int rtt_show_address(int argc, char **argv)
{
    rt_kprintf("RTT Control Block Detection Address is 0x%x\n", &_SEGGER_RTT);
    return RT_EOK;
}
#ifdef FINSH_USING_MSH
    FINSH_FUNCTION_EXPORT_ALIAS(rtt_show_address, __cmd_rtt_show_address, Show RTT Control Block Address.);
#endif
/*************************** End of file ****************************/
