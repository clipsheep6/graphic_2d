package com.huawei.config.datasource;

import com.mongodb.ConnectionString;
import com.mongodb.MongoClientSettings;
import com.mongodb.MongoCredential;
import com.mongodb.ReadPreference;
import com.mongodb.client.MongoClient;
import com.mongodb.client.internal.MongoClientImpl;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.BeanFactory;
import org.springframework.beans.factory.NoSuchBeanDefinitionException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.data.convert.CustomConversions;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.SimpleMongoClientDatabaseFactory;
import org.springframework.data.mongodb.core.convert.DbRefResolver;
import org.springframework.data.mongodb.core.convert.DefaultDbRefResolver;
import org.springframework.data.mongodb.core.convert.DefaultMongoTypeMapper;
import org.springframework.data.mongodb.core.convert.MappingMongoConverter;
import org.springframework.data.mongodb.core.mapping.MongoMappingContext;


/**
 * ci_info mongoDB配置类 读写均可。
 *
 * @since 2022-11-09
 */
@Configuration
@Slf4j
public class CiWriteMongoConfig {

    @Value(value = "${spring.data.mongodb.write-username}")
    private String userName;

    @Value(value = "${spring.data.mongodb.write-uri}")
    private String address;

    @Value(value = "${spring.data.mongodb.write-password}")
    private String password;

    @Value(value = "${spring.data.mongodb.ci-info-name}")
    private String dbName;

    @Autowired
    @Qualifier("ciWriteMongoClient")
    private MongoClient mongoClient;

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
    @Bean("ciWriteMongoClient")
    public MongoClient mongoClient() {
        //使用数据库名、用户名密码登录
        MongoCredential credential =
                MongoCredential.createCredential(userName, dbName, password.toCharArray());

        return new MongoClientImpl(getMongoClientOptions(credential), null);
    }


    /**
     * 注册mongodb操作类
     *
     * @param beanFactory 数据库连接工厂
     * @param context     context
     * @return MongoTemplate
     * @since 2022-11-09
     */
    @Bean("ciWriteMongoTemplate")
    public MongoTemplate mongoTemplate(MongoMappingContext context, BeanFactory beanFactory) {
        SimpleMongoClientDatabaseFactory simpleMongoClientDatabaseFactory = new SimpleMongoClientDatabaseFactory(mongoClient, dbName);
        DbRefResolver dbRefResolver = new DefaultDbRefResolver(simpleMongoClientDatabaseFactory);
        MappingMongoConverter mappingConverter = new MappingMongoConverter(dbRefResolver, context);
        try {
            mappingConverter.setCustomConversions(beanFactory.getBean(CustomConversions.class));
        } catch (NoSuchBeanDefinitionException ignore) {
            log.info(ignore.getMessage());
        }

        mappingConverter.setTypeMapper(new DefaultMongoTypeMapper(null));

        return new MongoTemplate(simpleMongoClientDatabaseFactory, mappingConverter);
    }
}