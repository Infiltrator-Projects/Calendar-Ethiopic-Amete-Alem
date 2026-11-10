// SPDX-License-Identifier: GPL-3.0-or-later
/* Independently installed calendar. No Calendar application ABI is exported. */
#include <infiltratr/core.h>
#include <infiltratr/temporal_calendar.h>
#include "calendar-engine-types.h"
#include "calendar-arithmetic.h"
#include "calendar-custom.h"
#include "calendar-historical.h"
#include "calendar-swedish.h"
#include "icu-calendar.h"
#include "catalogue.h"
#include "julian-day.h"
#include <string.h>

static bool provider_jdn_supported(int64_t jdn)
{
    return jdn >= INT64_C(2440588) - INT64_C(100000000) &&
           jdn <= INT64_C(2440588) + INT64_C(99999999);
}

static bool fields(const InfiltratrTemporalCalendarProvider *self,
    int64_t jdn, InfiltratrTemporalCalendarFields *result)
{
    CalendarCalendarFields value;
    (void)self;
    if (result == NULL || !provider_jdn_supported(jdn) ||
        !calendar_arithmetic_fields_from_jdn(RESOURCE_MODE, jdn, &value))
        return false;
    *result = (InfiltratrTemporalCalendarFields){
        value.year, value.month, value.day, value.auxiliary, value.special != FALSE };
    return true;
}
static bool format(const InfiltratrTemporalCalendarProvider *self,
    int64_t jdn, InfiltratrTemporalCalendarDatePart part,
    char *buffer, size_t capacity, size_t *length)
{
    gchar *text;
    CalendarCalendarFields validated_fields;
    (void)self;
    if (buffer == NULL || capacity == 0U || !provider_jdn_supported(jdn) ||
        part < INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_DAY ||
        part > INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL ||
        !calendar_arithmetic_fields_from_jdn(RESOURCE_MODE, jdn, &validated_fields))
        return false;
    /* Field conversion owns supported chronology. Formatting must not ask
     * ICU to invent dates outside a bounded arithmetic provider's range. */
    text = calendar_icu_format(RESOURCE_MODE, RESOURCE_KEYWORD, jdn, (CalendarDatePart)part);
    if (text == NULL || *text == '\0') { g_free(text); return false; }
    const size_t size = strlen(text);
    if (size >= capacity) { g_free(text); return false; }
    memcpy(buffer,text,size+1U);
    if (length != NULL) *length=size;
    g_free(text);
    return true;
}
static bool start(const InfiltratrTemporalCalendarProvider *self,
    int64_t jdn, int64_t *result)
{
    CalendarCalendarFields value;
    if (result == NULL || !fields(self,jdn,&(InfiltratrTemporalCalendarFields){0}))
        return false;
    const int64_t candidate=calendar_arithmetic_month_start(RESOURCE_MODE, jdn);
    if (!provider_jdn_supported(candidate) ||
        !calendar_arithmetic_fields_from_jdn(RESOURCE_MODE, candidate, &value)) return false;
    *result=candidate;
    return true;
}
static bool navigate(const InfiltratrTemporalCalendarProvider *self,
    int64_t jdn,int32_t amount,int64_t *result,bool by_year)
{
    CalendarCalendarFields value;
    if (result == NULL || !fields(self,jdn,&(InfiltratrTemporalCalendarFields){0}))
        return false;
    const int64_t candidate=(by_year ? calendar_arithmetic_add_years(RESOURCE_MODE, jdn, amount) : calendar_arithmetic_add_months(RESOURCE_MODE, jdn, amount));
    if (!provider_jdn_supported(candidate) ||
        !calendar_arithmetic_fields_from_jdn(RESOURCE_MODE, candidate, &value)) return false;
    *result=candidate;
    return true;
}
static bool periods(const InfiltratrTemporalCalendarProvider *self,
    int64_t jdn,int32_t amount,int64_t *result)
{ return navigate(self,jdn,amount,result,false); }
static bool years(const InfiltratrTemporalCalendarProvider *self,
    int64_t jdn,int32_t amount,int64_t *result)
{ return navigate(self,jdn,amount,result,true); }
static const InfiltratrTemporalResourceMetadata metadata = {
    sizeof(InfiltratrTemporalResourceMetadata),
    INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI,
    INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,RESOURCE_ID,INFILTRATR_TEMPORAL_DISPLAY_NAME,false,false,false
};
static const InfiltratrTemporalCalendarProvider provider = {
    sizeof(InfiltratrTemporalCalendarProvider),
    INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_ABI,RESOURCE_ID,
    fields,format,start,periods,years
};
#if defined(_WIN32)
#define RESOURCE_EXPORT __declspec(dllexport)
#else
#define RESOURCE_EXPORT __attribute__((visibility("default")))
#endif
RESOURCE_EXPORT const InfiltratrTemporalResourceMetadata *infiltratr_temporal_resource_metadata_v1(void);
RESOURCE_EXPORT const InfiltratrTemporalCalendarProvider *infiltratr_temporal_calendar_provider_v1(void);
RESOURCE_EXPORT const InfiltratrTemporalResourceMetadata *infiltratr_temporal_resource_metadata_v1(void) { return &metadata; }
RESOURCE_EXPORT const InfiltratrTemporalCalendarProvider *infiltratr_temporal_calendar_provider_v1(void) { return &provider; }
