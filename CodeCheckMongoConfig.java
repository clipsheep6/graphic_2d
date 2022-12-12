package com.huawei.config.datasource;

import com.mongodb.ConnectionString;
import com.mongodb.MongoClientSettings;
import com.mongodb.MongoCredential;
import com.mongodb.ReadPreference;
import com.mongodb.client.MongoClient;
import com.mongodb.client.internal.MongoClientImpl;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.SimpleMongoClientDatabaseFactory;
import org.springframework.data.mongodb.core.convert.MappingMongoConverter;

import javax.annotation.PostConstruct;


/**
 * codecheck mongoDB配置类
 *
 * @since 2022-11-09
 */
@Configuration
public class CodeCheckMongoConfig {

    @Value(value = "${spring.data.mongodb.username}")
    private String userName;

    @Value(value = "${spring.data.mongodb.uri}")
    private String address;

    @Value(value = "${spring.data.mongodb.password}")
    private String password;

    @Value(value = "${spring.data.mongodb.database}")
    private String dbName;


    @Autowired
    @Qualifier("codeCheckMongoClient")
    private MongoClient ciMongoClient;

    @Autowired
    private MappingMongoConverter mappingMongoConverter;

    @Autowired
    private SimpleMongoClientDatabaseFactory simpleMongoClientDatabaseFactory;

    @Autowired
    private MappingMongoConverter converter;

    /**
     * setMapKey
     */
    @PostConstruct
    public void setMapKey() {
        converter.setMapKeyDotReplacement("@dot@");
    }

    private MongoClientSettings getMongoClientOptions(MongoCredential credential) {
        MongoClientSettings.Builder builder = MongoClientSettings.builder();
        builder.applyConnectionString(new ConnectionString("mongodb://" + address));
        builder.credential(credential);
        return builder.readPreference(ReadPreference.primary()).build();
    }

    /**
     * mongoDB客户端
     *
     * @since 2022-11-09
     */
    @Bean("codeCheckMongoClient")
    public MongoClient mongoClient() {
        //使用数据库名、用户名密码登录
        MongoCredential credential = MongoCredential.createCredential(userName, dbName,
                password.toCharArray());

        return new MongoClientImpl(getMongoClientOptions(credential), null);
    }

    /**
     * 注册mongodb操作类
     *
     * @return MongoTemplate
     * @since 2022-11-09
     */
    @Bean("codeCheckMongoTemplate")
    public MongoTemplate mongoTemplate() {
        MongoTemplate mongoTemplate = new MongoTemplate(simpleMongoClientDatabaseFactory);
        return new MongoTemplate(simpleMongoClientDatabaseFactory, converter);
    }

    /**
     * simpleMongoClientDatabaseFactory
     * @return SimpleMongoClientDatabaseFactory
     */
    @Bean("simpleMongoClientDatabaseFactory")
    public SimpleMongoClientDatabaseFactory simpleMongoClientDatabaseFactory() {
        return new SimpleMongoClientDatabaseFactory(ciMongoClient, dbName);
    }

}