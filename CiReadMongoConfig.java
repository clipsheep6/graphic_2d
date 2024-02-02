package com.huawei.config.datasource;

import com.mongodb.ConnectionString;
import com.mongodb.MongoClientSettings;
import com.mongodb.MongoCredential;
import com.mongodb.ReadPreference;
import com.mongodb.client.MongoClient;
import com.mongodb.client.internal.MongoClientImpl;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.autoconfigure.condition.ConditionalOnClass;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.SimpleMongoClientDatabaseFactory;


/**
 * ci_info mongoDB配置类 只读
 *
 * @since 2022-11-09
 */
@Configuration
public class CiReadMongoConfig {

    @Value(value = "${spring.data.mongodb.read-username}")
    private String userName;

    @Value(value = "${spring.data.mongodb.read-uri}")
    private String address;

    @Value(value = "${spring.data.mongodb.read-password}")
    private String password;

    @Value(value = "${spring.data.mongodb.ci-info-name}")
    private String dbName;

    /**
     * mongoDB客户端
     */
    @Bean("ciMongoClient")
    @Primary
    public MongoClient mongoClient() {
        //使用数据库名、用户名密码登录
        MongoCredential credential =
                MongoCredential.createCredential(userName, dbName,password.toCharArray());

        return new MongoClientImpl(getMongoClientOptions(credential), null);
    }


    private MongoClientSettings getMongoClientOptions(MongoCredential credential) {
        MongoClientSettings.Builder builder = MongoClientSettings.builder();
        builder.applyConnectionString(new ConnectionString("mongodb://" + address));
        builder.credential(credential);
        return builder.readPreference(ReadPreference.primary()).build();
    }

    /**
     * 注册mongodb操作类
     *
     * @param mongoClient mongo客户端
     * @return MongoTemplate
     */
    @Bean("ciMongoTemplate")
    @Primary
    @ConditionalOnClass(MongoClient.class)
    public MongoTemplate mongoTemplate(MongoClient mongoClient) {
        return new MongoTemplate(new SimpleMongoClientDatabaseFactory(mongoClient, dbName));
    }
}