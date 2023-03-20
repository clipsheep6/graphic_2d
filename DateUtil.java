package com.huawei.utils;

import com.huawei.enums.DateEnums;
import org.apache.commons.lang3.time.DateFormatUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.format.DateTimeFormatter;
import java.time.temporal.ChronoUnit;
import java.time.temporal.TemporalAccessor;
import java.util.Calendar;
import java.util.Date;

@Component
public class DateUtil {

    private static final Logger logger = LoggerFactory.getLogger(DateUtil.class);

    private static final SimpleDateFormat dateFormat = DateEnums.DAY.getFormat();

    private static final String START_TIME_SUFFIX = "00:00:00";

    private static final String END_TIME_SUFFIX = "23:59:59";

    private static final String START_TIME_SUFFIX_1 = "000000";

    private static final String END_TIME_SUFFIX_1 = "235959";


    /**
     * 获取指定日期时间的前或后指定间隔的日期时间，并按指定时间样式返回
     *
     * @param timestamp     指定日期的时间戳，毫秒级
     * @param interval      指定间隔：0表示当天，-1表示前一天，1表示后一天，以此类推
     * @param returnPattern 指定时间样式字符串：例yyyy-MM-dd HH:mm:ss
     * @return 指定间隔及样式的日期
     * @since 2022/11/9
     */
    public static String getIntervalTime(long timestamp, int interval, String returnPattern) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(timestamp);
        calendar.add(Calendar.DAY_OF_MONTH, interval);
        return DateFormatUtils.format(calendar.getTime(), returnPattern);
    }

    /**
     * 获取任意时间前后小时的时间
     *
     * @param date     任意时间
     * @param interval -1表示前1小时，1表示后1小时
     * @return long time
     * @since 2022/11/9
     */
    public static Date getDateToNowDate(Date date, int interval) {
        Calendar calendar = Calendar.getInstance();
        /* HOUR_OF_DAY 指示一天中的小时 */
        calendar.setTime(date);
        calendar.add(Calendar.HOUR_OF_DAY, interval);
        return calendar.getTime();
    }

    /**
     * date格式转换ISO
     *
     * @param dateStr 日期字符串
     * @return Data数据
     * @since 2022/11/9
     */
    public static Date dateToISODate(String dateStr) {
        Date parse = null;
        try {
            SimpleDateFormat format = DateEnums.SECOND.getFormat();
            Date date = format.parse(dateStr);
            SimpleDateFormat ISODateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
            parse = ISODateFormat.parse(ISODateFormat.format(date));
        } catch (ParseException e) {
            logger.error("format string to date error : {}", String.valueOf(e));
        }
        return parse;
    }

    /**
     * 把时间字符串转换成 YYYY-MM-DD HH:MM:SS格式
     *
     * @param dateStr 字符串
     * @return YYYY-MM-DD HH:MM:SS
     * @since 2022/11/9
     */
    public static String dateToStandDate(String dateStr) {
        String parse = null;
        try {
            SimpleDateFormat format = DateEnums.SECOND.getFormat();
            Date date = format.parse(dateStr);
            parse = format.format(date);
        } catch (ParseException e) {
            logger.error("format string to date error : {}", String.valueOf(e));
        }
        return parse;
    }

    /**
     * 把时间YYYYMMDDHHMMSS字符串转换成 YYYY-MM-DD HH:MM:SS格式
     *
     * @param dateStr 字符串
     * @return YYYY-MM-DD HH:MM:SS
     * @since 2022/11/9
     */
    public static Date dateToStandDates(String dateStr) {
        Date parse = null;
        try {
            SimpleDateFormat format = DateEnums.SECOND_ONE.getFormat();
            Date date = format.parse(dateStr);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            parse = sdf.parse(sdf.format(date));
        } catch (ParseException e) {
            logger.error("format string to date error : {}", String.valueOf(e));
        }
        return parse;
    }

    /**
     * 时间YYYY-MM-DDTHH:MM:SS字符串 转换成  YYYY-MM-DD HH:MM:SS格式 去掉时间区T
     *
     * @param dateStr 字符串
     * @return YYYY-MM-DD HH:MM:SS
     * @since 2022/11/9
     */
    public static String dateZoneToStandDates(String dateStr) {
        String formatTime = "";
        try {
            SimpleDateFormat format = DateEnums.TIME_ZONE_T.getFormat();
            Date date = format.parse(dateStr);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            formatTime = sdf.format(date);
        } catch (ParseException e) {
            logger.error("format string to date error : {}", String.valueOf(e));
        }
        return formatTime;
    }

    /**
     * 将之间转为ISO格式的字符串
     *
     * @param date 日期时间
     * @return String 字符串格式时间
     * @since 2022/11/9
     */
    public static String ISODateString(Date date) {
        SimpleDateFormat format = DateEnums.ISO.getFormat();
        return format.format(date);
    }

    /**
     * 计算两个日期时间的时间差
     *
     * @param currentDate 开始日期时间
     * @param endDate     结束日期时间
     * @return String 字符串格式时间
     * @since 2022/11/9
     */
    public static long getDaysBetweenDate(Date currentDate, Date endDate) {
        return ChronoUnit.DAYS.between(currentDate.toInstant(), endDate.toInstant());
    }

    /**
     * 将之间转为ISO格式的字符串
     *
     * @param dateStr 字符串日期时间
     * @return Date 日期时间
     * @since 2022/11/9
     */
    public static Date getDate(String dateStr) {
        Date date = null;
        try {
            date = dateFormat.parse(dateStr);
        } catch (ParseException e) {
            logger.error("string format date error, string is {}", dateStr);
        }
        return date;
    }

    /**
     * getDate
     *
     * @param dateStr dateStr
     * @param pattern pattern
     * @return Date
     */
    public static Date getDate(String dateStr, DateEnums pattern) {
        Date date = null;
        try {
            date = pattern.getFormat().parse(dateStr);
        } catch (ParseException e) {
            logger.error("string format date error, string is {}", dateStr);
        }
        return date;
    }

    /**
     * validDate
     *
     * @param date    date
     * @param pattern pattern
     * @return boolean
     */
    public static boolean validDate(String date, DateEnums pattern) {
        try {
            SimpleDateFormat format = pattern.getFormat();
            // setLenient用于设置Calendar是否宽松解析字符串，如果为false，则严格解析；默认为true，宽松解析(02-29会解析成3-01)
            format.setLenient(false);
            format.parse(date);
        } catch (ParseException | NullPointerException e) {
            // 如果转换失败或者空指针，则不通过校验
            return false;
        }
        return true;
    }

    /**
     * 获取当前时间的string
     *
     * @return @since String 时间字符串
     */
    public static String getCurrentDateString() {
        SimpleDateFormat format = DateEnums.SECOND.getFormat();
        return format.format(new Date());
    }

    /**
     * 根据传入的日期枚举 返回响应格式的当前时间
     *
     * @param dateEnums 日期枚举
     * @return @since 返回响应格式的当前时间
     */
    public static String getCurrentDateByEnum(DateEnums dateEnums) {
        SimpleDateFormat format = dateEnums.getFormat();
        return format.format(new Date());
    }

    /**
     * 将带时区的时间格式字符串转化为其他时间格式
     *
     * @param dateStr 带时区的时间格式字符串
     *                例如: 2022-05-12T11:08:16+08:00,
     *                2022-05-12 11:08:16 +0800
     * @param pattern 时间格式
     * @return @since 时间字符串
     */
    public static String ISOZonedDateTimeToString(String dateStr, DateEnums pattern) {
        String parse = null;
        try {
            if (dateStr.contains("+08:00")) {
                DateTimeFormatter isoZonedDateTime = DateTimeFormatter.ISO_ZONED_DATE_TIME;
                TemporalAccessor temporalAccessor = isoZonedDateTime.parse(dateStr);
                DateTimeFormatter dateTimeFormatter = DateTimeFormatter.ofPattern(pattern.getPattern());
                parse = dateTimeFormatter.format(temporalAccessor);
            } else {
                SimpleDateFormat format = DateEnums.SECOND_ZONE.getFormat();
                Date date = format.parse(dateStr);
                parse = pattern.getFormat().format(date);
            }
        } catch (ParseException e) {
            logger.error("string format date error, string is {}", dateStr);
        }
        return parse;

    }

    /**
     * 获取前两天的初始时间
     *
     * @return @since
     */
    public static String getBeforeThreeDay() {
        SimpleDateFormat format = DateEnums.SECOND_ONE.getFormat();
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.add(Calendar.DATE, -2);
        String beforeThreeDay = format.format(calendar.getTime());
        return beforeThreeDay;
    }

    /**
     * 获取今天的结束时间
     *
     * @return @since currentDateEnd
     */
    public static String getCurrentDateEnd() {
        SimpleDateFormat format = DateEnums.SECOND_ONE.getFormat();
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.SECOND, 59);
        calendar.set(Calendar.MINUTE, 59);
        calendar.set(Calendar.HOUR_OF_DAY, 23);
        String currentDateEnd = format.format(calendar.getTime());
        return currentDateEnd;
    }

    /**
     * 获取三天前的日期时间
     *
     * @return @since
     */
    public static String getBeforeThreeDayString() {
        SimpleDateFormat format = DateEnums.SECOND.getFormat();
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.add(Calendar.DATE, -2);
        String beforeThreeDay = format.format(calendar.getTime());
        return beforeThreeDay;
    }

    /**
     * 获取七天前的日期
     *
     * @return String
     * @since 2022/11/9
     */
    public static String getBeforeSevenDayString() {
        SimpleDateFormat format = DateEnums.DAY.getFormat();
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.DATE, -7);
        return format.format(calendar.getTime());
    }
}
