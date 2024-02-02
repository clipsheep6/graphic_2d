package com.huawei.config.threadpool;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;

import java.util.concurrent.ThreadPoolExecutor;

/**
 * SyncDetailsThreadPool
 */
@Configuration
@EnableAsync
public class SyncDetailsThreadPool {

    private static final int INC_CORE_POOL_SIZE = 8;

    private static final int FULL_CORE_POOL_SIZE = 8;

    // io密集型的线程数：cpu核心线程数 * （1+线程等待时间/线程CPU时间） 或 cpu核心数乘2
    private static final int INC_MAX_POOL_SIZE = 8;

    private static final int FULL_MAX_POOL_SIZE = 8;

    // 允许线程空闲时间（单位：默认为秒）
    private static final int INC_KEEP_ALIVE_TIME = 5 * 60;

    // 定时任务 每20分钟调用一次
    private static final int FULL_KEEP_ALIVE_TIME = 21 * 60;

    // inc数据量小一些 队列可以小一点
    private static final int INC_QUEUE_CAPACITY = 5000;

    private static final int FULL_QUEUE_CAPACITY = 50000;

    // 线程名前缀
    private static final String INC_THREAD_NAME_PREFIX = "ci-thread-sync-inc-";

    private static final String Full_THREAD_NAME_PREFIX = "ci-thread-sync-full-";


    /**
     * syncIncThreadPool
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("syncIncThreadPool")
    public ThreadPoolTaskExecutor syncIncThreadPool(){
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(INC_CORE_POOL_SIZE);
        executor.setMaxPoolSize(INC_MAX_POOL_SIZE);
        executor.setKeepAliveSeconds(INC_KEEP_ALIVE_TIME);
        executor.setQueueCapacity(INC_QUEUE_CAPACITY);
        executor.setThreadNamePrefix(INC_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }

    /**
     * syncFullThreadPool
     * @return ThreadPoolTaskExecutor
     */
    @Bean("syncFullThreadPool")
    public ThreadPoolTaskExecutor syncFullThreadPool(){
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(FULL_CORE_POOL_SIZE);
        executor.setMaxPoolSize(FULL_MAX_POOL_SIZE);
        executor.setKeepAliveSeconds(FULL_KEEP_ALIVE_TIME);
        executor.setQueueCapacity(FULL_QUEUE_CAPACITY);
        executor.setThreadNamePrefix(Full_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }
}
