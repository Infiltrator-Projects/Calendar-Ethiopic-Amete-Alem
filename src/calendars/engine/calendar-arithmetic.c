// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 1993-2026 Shannon Smith

/*
 * Resource-owned arithmetic calendar engines.
 *
 * These rules intentionally replace ICU only where the calendar itself is a
 * deterministic arithmetic system. ICU remains the locale-formatting authority
 * for these resources; locale data is not owned by the Calendar application.
 *
 * Epochs and arithmetic follow the same civil-date/JDN convention used by the
 * rest of the resource pack: an integral JDN identifies a civil date at midnight.
 *
 * Invariants:
 * - this module owns rules and numeric continuation, never locale presentation;
 * - mode-specific month length, fields↔JDN conversion and month/year navigation
 *   share the same helpers so navigation cannot silently use a different leap
 *   rule from formatting;
 * - fixed-range tables such as Umm al-Qura reject navigation beyond their
 *   evidenced range instead of extrapolating invented dates;
 * - intermediate year/day arithmetic uses the checked/saturating Common
 *   primitives before narrowing back to public field widths.
 *
 * Family-specific conversion and rule sets stay together in private sibling
 * modules. Native filesystem calendars are not dispatched here: their complete
 * chronology travels with the executable resource module.
 */

#include "calendar-arithmetic.h"
#include "calendar-arithmetic-internal.h"

#include "julian-day.h"

static gboolean
arithmetic_mode_supported(CalendarCalendarMode mode)
{
    (void)mode;
    switch (mode)
    {
        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            return TRUE;
        default:
            return FALSE;
    }
}

static gint64
signed_year_from_fields(CalendarCalendarMode mode,
                        const CalendarCalendarFields *fields)
{
    (void)mode;
    switch (mode)
    {
        

        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            return calendar_arithmetic_coptic_signed_year_from_fields(
                mode, fields);

        

        

        

        default:
            return fields->year;
    }
}

static void
set_signed_year(CalendarCalendarMode mode,
                gint64 signed_year,
                CalendarCalendarFields *fields)
{
    (void)mode;
    switch (mode)
    {
        

        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            calendar_arithmetic_coptic_set_signed_year(mode, signed_year, fields);
            break;

        

        

        

        default:
            fields->year = signed_year;
            fields->auxiliary = 0;
            break;
    }
}

static gint
periods_per_year(CalendarCalendarMode mode)
{
    (void)mode;
    switch (mode)
    {
        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            return 13;
        default:
            return 12;
    }
}

static gint
month_length(CalendarCalendarMode mode,
             const CalendarCalendarFields *fields)
{
    (void)mode;
    const gint64 year = signed_year_from_fields(mode, fields);

    switch (mode)
    {
        

        

        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            return calendar_arithmetic_coptic_month_length(
                year, fields->month);

        

        default:
            return 0;
    }
}

static gint64
fields_to_jdn(CalendarCalendarMode mode,
              const CalendarCalendarFields *fields)
{
    (void)mode;
    const gint64 year = signed_year_from_fields(mode, fields);

    switch (mode)
    {
        

        

        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            return calendar_arithmetic_coptic_fields_to_jdn(
                mode, year, fields->month, fields->day);

        

        default:
            return CALENDAR_UNIX_EPOCH_JDN;
    }
}

gboolean
calendar_arithmetic_fields_from_jdn(
    CalendarCalendarMode mode,
    gint64 jdn,
    CalendarCalendarFields *fields)
{
    (void)mode;
    
    
    

    g_return_val_if_fail(fields != NULL, FALSE);
    *fields = (CalendarCalendarFields){ 0 };

    switch (mode)
    {
        

        

        

        case CALENDAR_CALENDAR_MODE_ETHIOPIC_AMETE_ALEM:
            return calendar_arithmetic_coptic_fields_from_jdn(
                mode, jdn, fields);

        

        default:
            return FALSE;
    }
}

gint64
calendar_arithmetic_month_start(
    CalendarCalendarMode mode,
    gint64 jdn)
{
    (void)mode;
    CalendarCalendarFields fields;

    if (!arithmetic_mode_supported(mode) ||
        !calendar_arithmetic_fields_from_jdn(mode, jdn, &fields))
    {
        return G_MAXINT64;
    }

    fields.day = 1;
    return fields_to_jdn(mode, &fields);
}

gint64
calendar_arithmetic_add_months(
    CalendarCalendarMode mode,
    gint64 jdn,
    gint amount)
{
    (void)mode;
    CalendarCalendarFields fields;
    gint64 year;
    gint64 serial;
    gint period_count;
    gint target_month;

    if (!arithmetic_mode_supported(mode) ||
        !calendar_arithmetic_fields_from_jdn(mode, jdn, &fields))
    {
        return G_MAXINT64;
    }

    year = signed_year_from_fields(mode, &fields);
    period_count = periods_per_year(mode);
    serial = calendar_i64_add_saturating(
        calendar_i64_add_saturating(
            calendar_i64_multiply_saturating(year, period_count),
            fields.month - 1),
        amount);
    year = calendar_floor_divide(serial, period_count);
    target_month = (gint)calendar_positive_modulo(serial, period_count) + 1;

    set_signed_year(mode, year, &fields);
    ;
    fields.month = target_month;
    fields.day = MIN(fields.day, month_length(mode, &fields));
    return fields_to_jdn(mode, &fields);
}

gint64
calendar_arithmetic_add_years(
    CalendarCalendarMode mode,
    gint64 jdn,
    gint amount)
{
    (void)mode;
    CalendarCalendarFields fields;
    gint64 year;

    if (!arithmetic_mode_supported(mode) ||
        !calendar_arithmetic_fields_from_jdn(mode, jdn, &fields))
    {
        return G_MAXINT64;
    }

    year = calendar_i64_add_saturating(
        signed_year_from_fields(mode, &fields), amount);
    ;
    set_signed_year(mode, year, &fields);
    fields.day = MIN(fields.day, month_length(mode, &fields));
    return fields_to_jdn(mode, &fields);
}
