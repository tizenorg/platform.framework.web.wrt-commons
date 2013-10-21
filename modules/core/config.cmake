# Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
#
# @file        config.cmake
# @author      Lukasz Marek (l.marek@samsung.com)
# @version     1.0
# @brief
#

# Set DPL core sources
SET(DPL_CORE_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/core/src/abstract_waitable_input_adapter.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/abstract_waitable_input_output_adapter.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/abstract_waitable_output_adapter.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/address.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/apply.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/assert.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/atomic.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/binary_queue.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/char_traits.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/colors.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/copy.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/errno_string.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/exception.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/fast_delegate.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/file_input.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/file_output.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/lexical_cast.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/mutable_task_list.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/mutex.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/named_base_pipe.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/named_output_pipe.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/noncopyable.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/once.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/read_write_mutex.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/recursive_mutex.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/scoped_dir.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/serialization.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/single_instance.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/singleton.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/semaphore.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/string.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/task.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/task_list.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/thread.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/type_list.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/union_cast.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/zip_input.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/application.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/main.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/waitable_event.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/waitable_handle.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/waitable_handle_watch_support.cpp
    ${PROJECT_SOURCE_DIR}/modules/core/src/generic_event.cpp
    PARENT_SCOPE
)


# Set DPL core headers
SET(DPL_CORE_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_input.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_input_output.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_output.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_waitable_input_adapter.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_waitable_input.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_waitable_input_output_adapter.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_waitable_input_output.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_waitable_output_adapter.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/abstract_waitable_output.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/address.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/aligned.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/apply.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/assert.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/atomic.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/availability.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/binary_queue.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/bool_operator.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/char_traits.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/colors.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/copy.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/enable_shared_from_this.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/errno_string.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/exception.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/fast_delegate.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/file_input.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/file_output.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/foreach.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/generic_event.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/lexical_cast.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/mutable_task_list.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/mutex.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/named_base_pipe.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/named_input_pipe.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/named_output_pipe.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/noreturn.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/noncopyable.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/once.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/optional.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/optional_typedefs.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/preprocessor.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/read_write_mutex.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/recursive_mutex.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scope_guard.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_resource.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_array.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_close.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_dir.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_fclose.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_free.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_ptr.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/scoped_gpointer.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/serialization.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/semaphore.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/shared_ptr.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/single_instance.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/singleton.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/singleton_impl.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/singleton_safe_impl.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/static_block.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/string.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/sstream.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/task.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/task_list.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/thread.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/type_list.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/union_cast.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/workaround.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/zip_input.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/application.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/framework_appcore.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/framework_efl.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/framework_vconf.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/main.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/waitable_event.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/waitable_handle.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/waitable_handle_watch_support.h
    PARENT_SCOPE
)

SET(DPL_CORE_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/core/include
    PARENT_SCOPE
)

SET(DPL_3RDPARTY_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/3rdparty/fastdelegate/FastDelegate.h
    ${PROJECT_SOURCE_DIR}/modules/core/include/dpl/3rdparty/fastdelegate/FastDelegateBind.h
    PARENT_SCOPE
)

