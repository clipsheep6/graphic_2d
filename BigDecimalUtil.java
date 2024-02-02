package com.huawei.utils;

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.SerializerProvider;

import java.io.IOException;
import java.math.BigDecimal;
import java.math.RoundingMode;

/**
 * 浮点型工具类
 *
 * @author lwx1021041
 * @since 2022/03/08
 */
public class BigDecimalUtil extends JsonSerializer<BigDecimal> {
    /**
     * 精确加法one+two
     *
     * @param one 源数字1
     * @param two 源数字2
     * @return 相加后的数字
     */
    public static double add(double one, double two) {
        return BigDecimal.valueOf(one).add(BigDecimal.valueOf(two)).doubleValue();
    }

    /**
     * 精确加法one+two
     *
     * @param one 源数字1
     * @param two 源数字2
     * @return 相加后的数字
     */
    public static double subtract(double one, double two) {
        return BigDecimal.valueOf(one).subtract(BigDecimal.valueOf(two)).doubleValue();
    }

    /**
     * 精确乘法one*two
     *
     * @param one 乘数
     * @param two 被乘数
     * @return 相乘后的数字
     */
    public static double multiply(double one, double two) {
        return BigDecimal.valueOf(one).multiply(BigDecimal.valueOf(two)).doubleValue();
    }

    /**
     * 精确除法one/two，保留指定小数点后几位（bit），四舍五入规则
     *
     * @param one 除数
     * @param two 被除数
     * @param bit 保留几位小数
     * @return 相除后的数字
     */
    public static double divideHalfUp(double one, double two, int bit) {
        return BigDecimal.valueOf(one).divide(BigDecimal.valueOf(two), bit, RoundingMode.HALF_UP).doubleValue();
    }

    /**
     * 将浮点型数据保留指定小数点后几位（bit），四舍五入规则
     *
     * @param num 源数字
     * @param bit 保留几位小数
     * @return 保留几位后的数字
     */
    public static double setBitHalfUp(double num, int bit) {
        return BigDecimal.valueOf(num).setScale(bit, RoundingMode.HALF_UP).doubleValue();
    }

    /**
     * 将字符串转为BigDecimal
     *
     * @param str
     * @return
     */
    public static BigDecimal stringToBigDecimal(String str) {
        if (str.contains("%")) {
            return new BigDecimal(str.replace("%", ""));
        } else if(str.contains("NA")) {
            return new BigDecimal(0);
        }else {
            return new BigDecimal(str);
        }
    }

    /**
     * jackson统一处理 BigDecimal类型四舍五入精确到两位小数
     *
     * @param value       BigDecimal
     * @param gen
     * @param serializers
     * @throws IOException
     */
    @Override
    public void serialize(BigDecimal value, JsonGenerator gen, SerializerProvider serializers) throws IOException {
        if (value != null) {
            gen.writeString(value.setScale(2, RoundingMode.HALF_UP).stripTrailingZeros().toPlainString());
        }
    }
}
