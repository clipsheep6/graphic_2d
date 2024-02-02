package com.huawei.utils;

import org.apache.commons.lang3.StringUtils;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Base64;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

public class CompressUtil {

    /**
     * 将字符串进行gzip压缩
     *
     * @param data 要被压缩的数据
     * @return 压缩后的数据
     * @throws IOException
     */
    public static String compress(String data) throws IOException {
        if (StringUtils.isEmpty(data)) {
            return null;
        }

        try (ByteArrayOutputStream out = new ByteArrayOutputStream();
                GZIPOutputStream gzip = new GZIPOutputStream(out)) {
            gzip.write(data.getBytes(StandardCharsets.UTF_8.toString()));
            gzip.finish();
            return Base64.getEncoder().encodeToString(out.toByteArray());
        }
    }

    /**
     * uncompress
     *
     * @param data 已被压缩的数据
     * @return 解压后的数据
     * @throws IOException when error occur
     */
    public static String uncompress(String data) throws IOException {
        byte[] decode = Base64.getDecoder().decode(data);
        try (ByteArrayOutputStream out = new ByteArrayOutputStream();
                ByteArrayInputStream in = new ByteArrayInputStream(decode);
                GZIPInputStream gzipStream = new GZIPInputStream(in)) {

            byte[] buffer = new byte[256];
            int byt;
            while ((byt = gzipStream.read(buffer)) >= 0) {
                out.write(buffer, 0, byt);
            }
            return out.toString(StandardCharsets.UTF_8.toString());
        }
    }

}