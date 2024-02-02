/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

package com.huawei.handler;

import org.apache.commons.lang3.StringUtils;
import org.apache.ibatis.type.JdbcType;
import org.apache.ibatis.type.MappedJdbcTypes;
import org.apache.ibatis.type.MappedTypes;
import org.apache.ibatis.type.TypeHandler;
import org.springframework.util.CollectionUtils;

import java.sql.CallableStatement;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/**
 * 将集合存入数据库varchar类型
 *
 * @since 2022.12.20
 * @description 类型转换器，用于数据库的varchar和Java中List<String>类型的相互转换
 */
@MappedJdbcTypes(JdbcType.VARCHAR)
@MappedTypes(List.class)
public class ListToVarcharTypeHandler implements TypeHandler<List<String>> {
    @Override
    public void setParameter(PreparedStatement preparedStatement, int num, List<String> strings, JdbcType jdbcType)
        throws SQLException {
        // 遍历List类型的入参，拼装为String类型，使用Statement对象插入数据库
        StringBuilder sb = new StringBuilder();
        if (!CollectionUtils.isEmpty(strings)) {
            for (int j = 0; j < strings.size(); j++) {
                if (j == strings.size() - 1) {
                    sb.append(strings.get(j));
                } else {
                    sb.append(strings.get(j)).append(";");
                }
            }
        }
        preparedStatement.setString(num, sb.toString());
    }

    @Override
    public List<String> getResult(ResultSet resultSet, String str) throws SQLException {
        // 获取String类型的结果，使用";"分割为List后返回
        String resultString = resultSet.getString(str);
        if (StringUtils.isNotEmpty(resultString)) {
            return Arrays.asList(resultString.split(";"));
        }
        return Collections.emptyList();
    }

    @Override
    public List<String> getResult(ResultSet resultSet, int num) throws SQLException {
        // 获取String类型的结果，使用";"分割为List后返回
        String resultString = resultSet.getString(num);
        if (StringUtils.isNotEmpty(resultString)) {
            return Arrays.asList(resultString.split(";"));
        }
        return Collections.emptyList();
    }

    @Override
    public List<String> getResult(CallableStatement callableStatement, int num) throws SQLException {
        // 获取String类型的结果，使用";"分割为List后返回
        String resultString = callableStatement.getString(num);
        if (StringUtils.isNotEmpty(resultString)) {
            return Arrays.asList(resultString.split(";"));
        }
        return Collections.emptyList();
    }
}
