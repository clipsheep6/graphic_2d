package com.huawei.config.threadpool;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;

import java.util.concurrent.ThreadPoolExecutor;

/**
 * ThreadPoolConfig
 */
@Configuration
@EnableAsync
public class ThreadPoolConfig {
    private static final int CORE_POOL_SIZE = 8;
    private static final int MAX_POOL_SIZE = 8;
    private static final int KEEP_ALIVE_TIME = 5 * 60;
    private static final int QUEUE_CAPACITY = 300;

    // 线程名前缀
    private static final String THREAD_NAME_PREFIX = "ci-thread-";
    private static final String INC_THREAD_NAME_PREFIX = "ci-thread-inc-";
    private static final String ISSUE_THREAD_NAME_PREFIX = "ci-thread-issue-";
    private static final String ISSUE_PROGRESS_THREAD_NAME_PREFIX = "ci-thread-issue-progress-";
    private static final String PR_THREAD_NAME_PREFIX = "ci-thread-pr-";
    private static final String COMMIT_THREAD_NAME_PREFIX = "ci-thread-commit-";
    private static final String COMMON_THREAD_NAME_PREFIX = "ci-thread-common-";
    private static final String THREAD_SYNC_INNER_CODECHECK_PREFIX = "ci-thread-inner-codecheck-";

    /**
     * 黄蓝区同步数据线程池
     *
     * @return threadPoolTaskExecutor
     * @since 2022/11/7 16:28
     */
    @Bean("poolTaskExecutorSyncInnerCodecheck")
    public ThreadPoolTaskExecutor poolTaskExecutorSyncInnerCodecheck() {
        ThreadPoolTaskExecutor threadPoolTaskExecutor = new ThreadPoolTaskExecutor();
        threadPoolTaskExecutor.setCorePoolSize(CORE_POOL_SIZE);
        threadPoolTaskExecutor.setMaxPoolSize(CORE_POOL_SIZE * 2);
        // 多余线程池保留时间20s
        threadPoolTaskExecutor.setKeepAliveSeconds(20);
        threadPoolTaskExecutor.setQueueCapacity(100);
        threadPoolTaskExecutor.setThreadNamePrefix(THREAD_SYNC_INNER_CODECHECK_PREFIX);
        // 失败的任务的加入队列重试
        threadPoolTaskExecutor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        threadPoolTaskExecutor.initialize();
        return threadPoolTaskExecutor;
    }


    /**
     * threadPoolTaskExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean
    public ThreadPoolTaskExecutor threadPoolTaskExecutor() {
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

    /**
     * incTaskExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("incTaskExecutor")
    public ThreadPoolTaskExecutor incTaskExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(CORE_POOL_SIZE);
        executor.setMaxPoolSize(16);
        executor.setKeepAliveSeconds(KEEP_ALIVE_TIME);
        executor.setQueueCapacity(500);
        executor.setThreadNamePrefix(INC_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }

    /**
     * issueExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("issueExecutor")
    public ThreadPoolTaskExecutor issueExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(CORE_POOL_SIZE);
        executor.setMaxPoolSize(MAX_POOL_SIZE);
        executor.setKeepAliveSeconds(KEEP_ALIVE_TIME);
        executor.setQueueCapacity(QUEUE_CAPACITY);
        executor.setThreadNamePrefix(ISSUE_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }

    /**
     * issueProgressExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("issueprogressExecutor")
    public ThreadPoolTaskExecutor issueProgressExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(CORE_POOL_SIZE);
        executor.setMaxPoolSize(MAX_POOL_SIZE);
        executor.setKeepAliveSeconds(KEEP_ALIVE_TIME);
        executor.setQueueCapacity(1000);
        executor.setThreadNamePrefix(ISSUE_PROGRESS_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }

    /**
     * prExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("prExecutor")
    public ThreadPoolTaskExecutor prExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(CORE_POOL_SIZE);
        executor.setMaxPoolSize(MAX_POOL_SIZE * 2);
        executor.setKeepAliveSeconds(KEEP_ALIVE_TIME);
        executor.setQueueCapacity(10000);
        executor.setThreadNamePrefix(PR_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }

    /**
     * commitExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("commitExecutor")
    public ThreadPoolTaskExecutor commitExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(13);
        executor.setAllowCoreThreadTimeOut(true);
        executor.setMaxPoolSize(13);
        executor.setKeepAliveSeconds(10);
        executor.setQueueCapacity(100000);
        executor.setThreadNamePrefix(COMMIT_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }

    /**
     * commonExecutor
     *
     * @return ThreadPoolTaskExecutor
     */
    @Bean("commonExecutor")
    public ThreadPoolTaskExecutor commonExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(4);
        executor.setAllowCoreThreadTimeOut(true);
        executor.setMaxPoolSize(8);
        executor.setKeepAliveSeconds(10);
        executor.setQueueCapacity(30);
        executor.setThreadNamePrefix(COMMON_THREAD_NAME_PREFIX);
        // 线程池对拒绝任务的处理策略
        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
        executor.initialize();
        return executor;
    }
}
