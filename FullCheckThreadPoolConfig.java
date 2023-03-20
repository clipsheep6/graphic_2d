package com.huawei.config.threadpool;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;

import java.util.concurrent.ThreadPoolExecutor;

/**
 * FullCheckThreadPoolConfig
 */
@Configuration
@EnableAsync
public class FullCheckThreadPoolConfig {

    private static final int CORE_POOL_SIZE = 5;

    private static final int MAX_POOL_SIZE = 8;

    private static final int KEEP_ALIVE_TIME = 300; // 允许线程空闲时间（单位：默认为秒）

    private static final int QUEUE_CAPACITY = 30;

    private static final String THREAD_NAME_PREFIX = "ci-thread-full"; // 线程名前缀

    /**
     * fullTaskExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("fullTaskExecutor")
    public ThreadPoolTaskExecutor fullTaskExecutor(){
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(CORE_POOL_SIZE);
        executor.setMaxPoolSize(MAX_POOL_SIZE);
        executor.setKeepAliveSeconds(KEEP_ALIVE_TIME);
        executor.setQueueCapacity(QUEUE_CAPACITY);
        executor.setThreadNamePrefix(THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }
}
