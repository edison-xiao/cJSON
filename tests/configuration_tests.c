/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

static void create_configuration_should_create_a_configuration(void)
{
    cJSON *json = NULL;
    internal_configuration *configuration = NULL;
    int userdata = 1;

    json = cJSON_Parse("{\"buffer_size\":1024,\"format\":false,\"case_sensitive\":false,\"allow_data_after_json\":false}");
    TEST_ASSERT_NOT_NULL(json);
    configuration = (internal_configuration*)cJSON_CreateConfiguration(json, NULL, &userdata);
    cJSON_Delete(json);
    json = NULL;
    TEST_ASSERT_NOT_NULL(configuration);
    TEST_ASSERT_EQUAL_MESSAGE(configuration->buffer_size, 1024, "buffer_size has an incorrect value.");
    TEST_ASSERT_FALSE_MESSAGE(configuration->format, "format has an incorrect value.");
    TEST_ASSERT_FALSE_MESSAGE(configuration->case_sensitive, "case_sensitive has an incorrect value.");
    TEST_ASSERT_FALSE_MESSAGE(configuration->allow_data_after_json, "allow_data_after_json has an incorrect value.");
    TEST_ASSERT_TRUE_MESSAGE(configuration->userdata == &userdata, "Incorrect userdata");
    TEST_ASSERT_TRUE_MESSAGE(global_allocate_wrapper == configuration->allocators.allocate, "Wrong malloc.");
    TEST_ASSERT_TRUE_MESSAGE(global_reallocate_wrapper == configuration->allocators.reallocate, "Wrong realloc.");
    TEST_ASSERT_TRUE_MESSAGE(global_deallocate_wrapper == configuration->allocators.deallocate, "Wrong realloc.");

    free(configuration);
}

static void create_configuration_should_work_with_an_empty_object(void)
{
    internal_configuration *configuration = NULL;
    int userdata = 1;

    configuration = (internal_configuration*)cJSON_CreateConfiguration(NULL, NULL, &userdata);
    TEST_ASSERT_NOT_NULL(configuration);
    TEST_ASSERT_EQUAL_MESSAGE(configuration->buffer_size, 256, "buffer_size has an incorrect value.");
    TEST_ASSERT_TRUE_MESSAGE(configuration->format, "format has an incorrect value.");
    TEST_ASSERT_TRUE_MESSAGE(configuration->case_sensitive, "case_sensitive has an incorrect value.");
    TEST_ASSERT_TRUE_MESSAGE(configuration->allow_data_after_json, "allow_data_after_json has an incorrect value.");
    TEST_ASSERT_TRUE_MESSAGE(configuration->userdata == &userdata, "Incorrect userdata");
    TEST_ASSERT_TRUE_MESSAGE(global_allocate_wrapper == configuration->allocators.allocate, "Wrong malloc.");
    TEST_ASSERT_TRUE_MESSAGE(global_reallocate_wrapper == configuration->allocators.reallocate, "Wrong realloc.");
    TEST_ASSERT_TRUE_MESSAGE(global_deallocate_wrapper == configuration->allocators.deallocate, "Wrong free.");

    free(configuration);
}

static void* custom_allocator(size_t size, void *userdata)
{
    *((size_t*)userdata) = size;
    return malloc(size);
}
static void custom_deallocator(void *pointer, void *userdata)
{
    *((size_t*)userdata) = (size_t)pointer;
    free(pointer);
}

static void create_configuration_should_take_custom_allocators(void)
{
    internal_configuration *configuration = NULL;
    cJSON_Allocators allocators = {custom_allocator, custom_deallocator, NULL};
    size_t userdata = 0;

    configuration = (internal_configuration*)cJSON_CreateConfiguration(NULL, &allocators, &userdata);
    TEST_ASSERT_NOT_NULL(configuration);
    TEST_ASSERT_EQUAL_MESSAGE(userdata, sizeof(internal_configuration), "custom allocator wasn't run properly.");
    TEST_ASSERT_TRUE_MESSAGE(custom_allocator == configuration->allocators.allocate, "Wrong allocator.");
    TEST_ASSERT_TRUE_MESSAGE(custom_deallocator == configuration->allocators.deallocate, "Wrong deallocator.");
    TEST_ASSERT_NULL_MESSAGE(configuration->allocators.reallocate, "Reallocator is not null");

    custom_deallocator(configuration, &userdata);
}

static void configuration_change_allocators_should_change_allocators(void)
{
    internal_configuration *configuration = NULL;
    cJSON_Allocators allocators = {custom_allocator, custom_deallocator, NULL};
    size_t userdata = 0;

    configuration = (internal_configuration*)cJSON_CreateConfiguration(NULL, &allocators, &userdata);
    TEST_ASSERT_NOT_NULL(configuration);

    configuration = (internal_configuration*)cJSON_ConfigurationChangeAllocators(configuration, allocators);
    TEST_ASSERT_NOT_NULL(configuration);
    TEST_ASSERT_TRUE_MESSAGE(custom_allocator == configuration->allocators.allocate, "Wrong allocator.");
    TEST_ASSERT_TRUE_MESSAGE(custom_deallocator == configuration->allocators.deallocate, "Wrong deallocator.");
    TEST_ASSERT_NULL_MESSAGE(configuration->allocators.reallocate, "Reallocator is not null");

    custom_deallocator(configuration, &userdata);
}

static void configuration_change_userdata_should_change_userdata(void)
{
    internal_configuration *configuration = NULL;
    size_t userdata = 0;
    configuration = (internal_configuration*)cJSON_CreateConfiguration(NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(configuration);

    configuration = (internal_configuration*)cJSON_ConfigurationChangeUserdata(configuration, &userdata);
    TEST_ASSERT_TRUE_MESSAGE(configuration->userdata == &userdata, "Userdata is incorrect.");

    free(configuration);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(create_configuration_should_create_a_configuration);
    RUN_TEST(create_configuration_should_work_with_an_empty_object);
    RUN_TEST(create_configuration_should_take_custom_allocators);
    RUN_TEST(configuration_change_allocators_should_change_allocators);
    RUN_TEST(configuration_change_userdata_should_change_userdata);

    return UNITY_END();
}
