package com.huawei.config.datasource;

import com.alibaba.druid.pool.DruidDataSource;
import lombok.extern.slf4j.Slf4j;
import org.apache.shardingsphere.driver.api.ShardingSphereDataSourceFactory;
import org.apache.shardingsphere.infra.config.algorithm.ShardingSphereAlgorithmConfiguration;
import org.apache.shardingsphere.sharding.api.config.ShardingRuleConfiguration;
import org.apache.shardingsphere.sharding.api.config.rule.ShardingTableRuleConfiguration;
import org.apache.shardingsphere.sharding.api.config.strategy.sharding.StandardShardingStrategyConfiguration;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import javax.sql.DataSource;
import java.sql.SQLException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;


/**
 * @Description 门禁 mysql数据库连接
 * @ClassName com.huawei.config
 * @since 2022/11/4  11:07
 **/
@Slf4j
@Configuration
public class DataSourceConfig {

    @Value("${spring.datasource.driver-class-name}")
    private String driverClassName;

    @Value("${spring.datasource.jdbc-url}")
    private String url;

    @Value("${spring.datasource.username}")
    private String username;

    @Value("${spring.datasource.password}")
    private String password;

    @Bean
    public DataSource dataSource() {
        DataSource dataSource = null;
        // 配置数据源
        Map<String, DataSource> dataSourceMap = new HashMap<>();
        DruidDataSource CI_CODE_CHECK_ACCESS = new DruidDataSource();
        CI_CODE_CHECK_ACCESS.setDriverClassName(driverClassName);
        CI_CODE_CHECK_ACCESS.setUrl(url);
        CI_CODE_CHECK_ACCESS.setUsername(username);
        CI_CODE_CHECK_ACCESS.setPassword(password);
        dataSourceMap.put("codecheckAccessAccess", CI_CODE_CHECK_ACCESS);
        // 2022/11/17--->  目前代码检查问题详情使用的还是mongodb,如果后期有迁移到mysql的打算，则使用以下方式来分表。
        // 配置 门禁codecheck代码检查 详情表分表规则
        ShardingTableRuleConfiguration orderTableRuleConfig = new ShardingTableRuleConfiguration("t_task_inc_result_detail",
                "codecheckAccessAccess.t_task_inc_result_detail_${0..9}");
        // 配置分表策略,按照哪个字段来分表
        StandardShardingStrategyConfiguration standardShardingStrategyConfiguration = new StandardShardingStrategyConfiguration("snowid",
                "tableShardingAlgorithm");
        orderTableRuleConfig.setTableShardingStrategy(standardShardingStrategyConfiguration);
        // 配置分片规则
        ShardingRuleConfiguration shardingRuleConfig = new ShardingRuleConfiguration();
        // 设置单个或者多个表的分表规则
        shardingRuleConfig.getTables().add(orderTableRuleConfig);
        // 配置分表算法 门禁这块分10个表
        Properties tableShardingAlgorithmrProps = new Properties();
        // 设置当前 数据入库方式
        tableShardingAlgorithmrProps.setProperty("algorithm-expression", "t_task_inc_result_detail_${snowid % 10}");
        // 设置允许范围查询(必要，默认关闭 手动打开)
        tableShardingAlgorithmrProps.setProperty("allow-range-query-with-inline-sharding", String.valueOf(true));
        ShardingSphereAlgorithmConfiguration inline = new ShardingSphereAlgorithmConfiguration("INLINE", tableShardingAlgorithmrProps);
        shardingRuleConfig.getShardingAlgorithms().put("tableShardingAlgorithm", inline);
        try {
            dataSource = ShardingSphereDataSourceFactory.createDataSource(dataSourceMap, Collections.singleton(shardingRuleConfig),
                    new Properties());
        } catch (SQLException e) {
            log.error(e.getMessage());
        }
        return dataSource;
    }

}
