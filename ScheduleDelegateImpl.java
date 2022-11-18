package com.huawei.impl;

import com.huawei.ci.common.codecheck.RestCodeCheckUtils;
import com.huawei.ci.common.entity.CodeCheckIssueDefect;
import com.huawei.ci.common.entity.CodeCheckIssueFragment;
import com.huawei.ci.common.entity.CodeCheckResultDetails;
import com.huawei.ci.common.entity.CodeCheckResultSummary;
import com.huawei.ci.common.pojo.vo.Response;
import com.huawei.ci.common.utils.JsonUtils;
import com.huawei.entity.pojo.CodeCheckResultSummaryVo;
import com.huawei.entity.pojo.CodeCheckTaskIncVo;
import com.huawei.entity.vo.ciinfo.codecheck.*;
import com.huawei.entity.vo.ciinfo.event.CodeCheckDevCloud;
import com.huawei.entity.vo.ciinfo.event.CodeCheckDevVo;
import com.huawei.entity.vo.codecheck.eventModule.CustomParameterVo;
import com.huawei.entity.vo.codecheck.fossscan.FossScan;
import com.huawei.entity.vo.codecheck.fossscan.FossscanFragment;
import com.huawei.enums.*;
import com.huawei.mapper.TaskIncDetailMapper;
import com.huawei.operation.*;
import com.huawei.service.ScheduleService;
import com.huawei.utils.CompressUtil;
import com.huawei.utils.RedisOperateUtils;
import com.huawei.utils.TimeUtils;
import com.huawei.utils.UUIDSnowFlakeUtil;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.collections4.CollectionUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.lang3.time.DateFormatUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;
import org.springframework.stereotype.Component;

import java.io.IOException;
import java.net.URLDecoder;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

import static com.huawei.enums.CodeCheckAccessConstant.APPLICATION_URL;
import static com.huawei.enums.CodeCheckConstants.*;

/**
 * @Description 定时任务业务层
 * @ClassName com.huawei.impl
 * @since 2022/11/9  14:04
 **/
@Component
@Slf4j
public class ScheduleDelegateImpl implements ScheduleService {

    // 日志记录
    private static final Logger logger = LoggerFactory.getLogger(ScheduleDelegateImpl.class);

    private static final SimpleDateFormat second = DateEnums.SECOND.getFormat();

    private static final SimpleDateFormat secondOne = DateEnums.SECOND_ONE.getFormat();

    private final static String EXCLUDE_PROJECT_NAME = "https://gitee.com/ohpg/tsc.git";

    private final static String ARK_X_PROJECT = "https://gitee.com/arkui-x";

    private final static String APPLICATION_ARK_URL = "http://ci.arkui-x.cn";

    private final static String OHOS_REPO_INFO_SUFFIX = ".git";

    @Autowired
    private TaskIncOperation taskIncOperation;

    @Autowired
    @Qualifier(value = "incTaskExecutor")
    private ThreadPoolTaskExecutor incTaskExecutor;

    @Autowired
    @Qualifier(value = "syncIncThreadPool")
    private ThreadPoolTaskExecutor syncIncThreadPool;

    @Autowired
    @Qualifier(value = "poolTaskExecutorSyncInnerCodecheck")
    private ThreadPoolTaskExecutor poolTaskExecutorSyncInnerCodecheck;

    @Autowired
    private RestCodeCheckUtils restCodeCheckUtils;

    @Autowired
    private IncResultSummaryOperation incResultSummaryOperation;

    @Autowired
    private EventCodeCheckOperation eventCodeCheckOperation;

    @Autowired
    private RedisOperateUtils redisOperateUtils;

    @Autowired
    private CodeCheckOperation codeCheckOperation;

    @Autowired
    private IncResultDetailsOperation incResultDetailsOperation;

    @Autowired
    private FossScanOperation fossScanOperation;

    @Autowired
    private TimeUtils timeUtils;

    @Autowired
    private CodeCheckDevOperation codeCheckDevOperation;

    @Autowired
    private CustomParameterOperation customParameterOperation;

    @Autowired
    private UUIDSnowFlakeUtil uuidSnowFlakeUtil;

    @Autowired
    private TaskIncDetailMapper taskIncDetailMapper;

    @Override
    public Response getEventTaskIncCodecheck() {
        Response<Object> response = new Response<>();
        long timeNow = new Date().getTime();
        // 超时时间+周期时间 +1 *3 分钟容错 +3   判定超时状态30 +1 *3 +3 = 36
        String dateBeforeStr = timeUtils.dateToTime(new Date(timeNow - (36 * TimeUtils.MINUTE)));
        // 36分钟之内的所有 黄区任务进行筛选同步
        List<CodeCheckDevCloud> innerCodeCheckVo = codeCheckDevOperation.getInnerCodeCheckVo(dateBeforeStr);
        CustomParameterVo customParameterByConfiguration = customParameterOperation.getCustomParameterByConfiguration(OHOS_REPO_CONFIG_KEY);
        List<String> reposForInner = (List<String>) customParameterByConfiguration.getParameters().get("gitUrls");
        logger.info("------ start to sync codecheck total involved with inner codecheck and community ----- ");
        // 线程池同步
        poolTaskExecutorSyncInnerCodecheck.execute(() -> {
            if (CollectionUtils.isNotEmpty(innerCodeCheckVo)) {
                for (CodeCheckDevCloud codeCheckDevCloud : innerCodeCheckVo) {
                    String devKey = RedisConstant.DEV_LOCK_PREFIX + codeCheckDevCloud.getUuid();
                    try {
                        String DevLock = LOCK;
                        if (!redisOperateUtils.setIfAbsent(devKey, DevLock, 10, TimeUnit.SECONDS)) {
                            while (LOCK.equalsIgnoreCase(DevLock)) {
                                DevLock = redisOperateUtils.getAndSet(devKey, DevLock).toString();
                            }
                            dealWithCurrentPrStaticCodeCheck(codeCheckDevCloud, reposForInner, timeNow);
                            logger.info("-- End sync Y codecheck involved with outside, and current uuid is: {} --", codeCheckDevCloud.getUuid());
                        }
                    } finally {
                        redisOperateUtils.set(devKey, UNLOCK);
                    }
                }
            }
        });
        logger.info("------ End to sync codecheck total involved with inner codecheck and community ------- ");
        response.setCode(String.valueOf(200));
        response.setMessage(SUCCESS);
        return response;
    }


    /**
     * 处理门禁结果
     *
     * @param codeCheckDevCloud 黄区门禁检查数据
     * @param reposForInner     黄区内部仓
     * @param timeNow           当前时间
     */
    private void dealWithCurrentPrStaticCodeCheck(CodeCheckDevCloud codeCheckDevCloud, List<String> reposForInner, Long timeNow) {
        long timeStampForInnerStart = 0L;
        String uuid = codeCheckDevCloud.getUuid();
        EventModel eventByUuid = eventCodeCheckOperation.getEventByUuid(uuid);
        List<CodeCheckResultSummaryVo> incResultSummaryList = incResultSummaryOperation.getSummaryByUuid(uuid);
        CodeCheckInfo codeCheckInfo = null;
        if (Objects.nonNull(eventByUuid)) {
            // 确保时间正常
            timeStampForInnerStart = checkTimeExist(codeCheckDevCloud, timeStampForInnerStart, eventByUuid);
            // 静态检查标准
            codeCheckInfo =JsonUtils.fromJson(JsonUtils.toJson(eventByUuid.getCodeCheckInfo()), CodeCheckInfo.class);
        }
        // 检查项详情
        Map<String, List<CodeCheckDevVo>> subCodeCheckList = codeCheckDevCloud.getSubCodeCheckList();
        int countPRCheckInfo = subCodeCheckList.values().stream().mapToInt(List::size).sum();
        if (Objects.nonNull(codeCheckDevCloud.getTotalResult())) {
            logger.info("-- start sync Y codecheck result involved with outside：{} ", codeCheckDevCloud.getUuid());
            if (Objects.nonNull(codeCheckInfo)) {
                CodeCheckInfo finalCodeCheckInfo = codeCheckInfo;
                // 如果出现map存在但是实际为空
                if (!subCodeCheckList.isEmpty()) {
                    subCodeCheckList.forEach((HK, VA) -> {
                        Set<String> innerCodeCheckSet = new HashSet<>();
                        String gitUrlInvolvedWithCurrentPR = dealWithCurrentPR(HK);
                        String prUrl = convertEncodingFormat(HK);
                        if (StringUtils.isNotBlank(gitUrlInvolvedWithCurrentPR) && reposForInner.contains(gitUrlInvolvedWithCurrentPR)
                                && !EXCLUDE_PROJECT_NAME.equals(gitUrlInvolvedWithCurrentPR) && !gitUrlInvolvedWithCurrentPR.startsWith(ARK_PROJECT)) {
                            logger.info("------- start to synchronize total result for current PR: {} , uuid is: {}  ------- ", prUrl, uuid);
                            // 标准同步
                            checkCurrentPRUnderStandard(subCodeCheckList, VA, countPRCheckInfo, innerCodeCheckSet, HK, prUrl,
                                    codeCheckDevCloud, finalCodeCheckInfo);
                        } else {
                            // 外部同步 只同步对应的蓝区
                            logger.info("-- current pr: {} ,which is not ned to sync Y-codecheck-result --", prUrl);
                            doSynchronizedWithAll(HK, prUrl, codeCheckDevCloud, PASS, finalCodeCheckInfo);
                        }
                    });
                } else {
                    // 触发了静态检查，但是检查项为空的情况下。黄区codecheck同步过来的检查项异常
                    doSyncDevError(uuid, reposForInner, incResultSummaryList);
                }
            } else {
                logger.info("-- current pr which uuid is: {}, has do not trigger static codecheck yet --", uuid);
                // 未触发静态检查
                doSynchronizedWithWrongCheckForInner(codeCheckDevCloud.getUuid());
            }
        } else {
            if (Objects.nonNull(eventByUuid) && CollectionUtils.isNotEmpty(incResultSummaryList)) {
                // 执行中
                checkIfCodeCheckOverTime(reposForInner, incResultSummaryList, codeCheckInfo, timeStampForInnerStart, timeNow, uuid, codeCheckDevCloud);
            }
        }
        // 总结果
        if (Objects.isNull(codeCheckInfo)) {
            fedBackStaticTotalResult(PASS, uuid);
        } else {
            if (Objects.nonNull(codeCheckDevCloud.getTotalResult()) && CollectionUtils.isNotEmpty(incResultSummaryList)) {
                checkEffectiveResult(subCodeCheckList, reposForInner, codeCheckInfo, incResultSummaryList, uuid, codeCheckDevCloud);
            } else {
                checkTotalResultIfTimeOut(timeNow, reposForInner, incResultSummaryList, timeStampForInnerStart, uuid);
            }
        }
    }

    /**
     * 检查当前任务总结果是否有超时的pr存在
     *
     * @param uuid                   uuid
     * @param byUuid                 pr
     * @param timeStampForInnerStart 开始时间
     */
    private void checkTotalResultIfTimeOut(Long timeNow, List<String> reposForInner, List<CodeCheckResultSummaryVo> byUuid, long timeStampForInnerStart, String uuid) {
        if ((timeNow - timeStampForInnerStart) > TimeUtils.HOUR / 2) {
            byUuid.forEach(currentPr -> {
                if (Objects.nonNull(currentPr)) {
                    String mrUrl = currentPr.getMrUrl();
                    String repoGitUrl = mrUrl.substring(0, mrUrl.indexOf("/pulls")) + "git";
                    if (reposForInner.contains(repoGitUrl)) {
                        // 某条超时，则总结果超时
                        doSynchronizedWithYCodeCheckTimeOut(timeStampForInnerStart, uuid);
                    }
                }
            });
        }
    }

    private void doSynchronizedWithYCodeCheckTimeOut(long timeStampForInnerStart, String uuid) {
        if (timeStampForInnerStart != 0L) {
            logger.info("-- inner codecheck time out,and uuid is:{}", uuid);
            String resultForCurrentPrOfInner = AssistantConstants.YELLOW_CODECHECK_TIME_OUT;
            // 超时
            fedBackStaticTotalResultForYTimeOut(resultForCurrentPrOfInner, uuid);
        }
    }

    private void fedBackStaticTotalResultForYTimeOut(String resultForCurrentPrOfInner, String uuid) {
        CICodeCheck ciCodeCheckByUuid = codeCheckOperation.getCICodeCheckByUuid(uuid);
        CICodeCheck ciCodeCheck = new CICodeCheck();
        if (Objects.nonNull(ciCodeCheckByUuid)) {
            BeanUtils.copyProperties(ciCodeCheckByUuid, ciCodeCheck);
            // 保证蓝区静态检查的结果已经回写完成
            if (Objects.nonNull(ciCodeCheck.getTotalResult())) {
                ciCodeCheck.setTotalResult(resultForCurrentPrOfInner);
                codeCheckOperation.findAndReplacePRInnerResult(uuid, ciCodeCheck);
            }
        }
    }

    /**
     * 融合结果
     */
    private void checkEffectiveResult(Map<String, List<CodeCheckDevVo>> subCodeCheckList, List<String> reposForInner, CodeCheckInfo codeCheckInfo,
                                      List<CodeCheckResultSummaryVo> incResultSummaryList, String uuid, CodeCheckDevCloud codeCheckDevCloud) {
        String totalResultForBCheck;
        String totalResultForYCheck = doFilterForInnerReposTotalResult(subCodeCheckList, reposForInner, codeCheckInfo, incResultSummaryList, codeCheckDevCloud);
        // 只触发失败项流水线，蓝区默认通过
        if (Objects.nonNull(codeCheckInfo)) {
            totalResultForBCheck = doFilterForCommunityCheckResult(incResultSummaryList);
        } else {
            totalResultForBCheck = PASS;
        }
        // 完成状态才进行同步操作
        if (!totalResultForYCheck.equals(RUNNING)) {
            fedBackStaticTotalResult(doJudgmentForCurrentIncEventAllPR(totalResultForBCheck, totalResultForYCheck), uuid);
        }
    }

    /**
     * 判断当前合入请求的码云回写黄蓝区总结果
     *
     * @param totalResultForBCheck 蓝区总结果
     * @param totalResultForYCheck 黄区总结果
     */
    private String doJudgmentForCurrentIncEventAllPR(String totalResultForBCheck, String totalResultForYCheck) {
        if (totalResultForBCheck.equals(PASS) && totalResultForYCheck.equals(PASS)) {
            return PASS;
        } else {
            return FAILED;
        }
    }


    private void fedBackStaticTotalResult(String currentPRStaticResult, String uuid) {
        fedBackCodeCheckStaticTotalResult(uuid, currentPRStaticResult);
    }

    private void fedBackCodeCheckStaticTotalResult(String uuid, String currentPRStaticResult) {
        CICodeCheck ciCodeCheckByUuid = codeCheckOperation.getCICodeCheckByUuid(uuid);
        // 蓝区数据已完成才会回写黄区数据
        if (Objects.nonNull(ciCodeCheckByUuid) && Objects.nonNull(ciCodeCheckByUuid.getTotalResult())) {
            ciCodeCheckByUuid.setTotalResult(currentPRStaticResult);
            codeCheckOperation.findAndReplacePRInnerResult(uuid, ciCodeCheckByUuid);
        }
    }

    /**
     * 执行中条件分类
     */
    private void checkIfCodeCheckOverTime(List<String> reposForInner, List<CodeCheckResultSummaryVo> incResultSummaryList, CodeCheckInfo codeCheckInfo,
                                          long timeStampForInnerStart, Long timeNow, String uuid, CodeCheckDevCloud codeCheckDevCloud) {
        if (CollectionUtils.isNotEmpty(incResultSummaryList)) {
            incResultSummaryList.stream().forEach(codeCheckResultSummaryVo -> {
                String prUrl = codeCheckResultSummaryVo.getMrUrl();
                String gitUrl = prUrl.substring(0, prUrl.indexOf("/pulls")) + ".git";
                // 符合内部 标准仓
                if (reposForInner.contains(gitUrl) && Objects.nonNull(codeCheckInfo)
                        && !EXCLUDE_PROJECT_NAME.equals(gitUrl) && !gitUrl.startsWith(ARK_PROJECT)) {
                    checkCurrentPRIfTimeOut(gitUrl, prUrl, reposForInner, timeStampForInnerStart, uuid, timeNow);
                } else {
                    doSynchronizedWithAll(prUrl, gitUrl, codeCheckDevCloud, PASS, codeCheckInfo);
                }
            });
        }
    }

    /**
     * 同步超时
     */
    private void checkCurrentPRIfTimeOut(String gitUrl, String prUrl,
                                         List<String> reposForInner, long timeStampForInnerStart, String uuid, Long timeNow) {
        if (Objects.nonNull(prUrl)) {
            if (reposForInner.contains(gitUrl)) {
                doJudgmentForCurrentPRIfTimeOut(prUrl, timeStampForInnerStart, uuid, null, timeNow);
            }
        }
    }

    private void doJudgmentForCurrentPRIfTimeOut(String mrUrl, long timeStampForInnerStart, String uuid, String codecheckStatus, long timeNow) {
        if (timeStampForInnerStart != 0L) {
            if ((timeNow - timeStampForInnerStart) > TimeUtils.HOUR / 2) {
                logger.info("-- inner codecheck time out,and uuid is:{}", uuid);
                fedBackStaticForTimeOut(AssistantConstants.YELLOW_CODECHECK_TIME_OUT, uuid, mrUrl);
            }
        }
    }

    private void fedBackStaticForTimeOut(String resultForCurrentPrOfInner, String uuid, String mrUrl) {
        updatePrCodeCheckStatus(resultForCurrentPrOfInner, uuid, mrUrl);

    }

    private void updatePrCodeCheckStatus(String resultForCurrentPrOfInner, String uuid, String mrUrl) {
        CICodeCheck ciCodeCheckByUuid = codeCheckOperation.getCICodeCheckByUuid(uuid);
        CICodeCheck ciCodeCheck = new CICodeCheck();
        // 保证数据有效
        if (Objects.nonNull(ciCodeCheckByUuid)) {
            BeanUtils.copyProperties(ciCodeCheckByUuid, ciCodeCheck);
            // 确认蓝区数据已经回写成功
            if (Objects.nonNull(ciCodeCheck.getCodeCheckResult()) && Objects.nonNull(ciCodeCheck.getTotalResult())) {
                Map<String, CodeCheckResultVo> codeCheckResult = ciCodeCheck.getCodeCheckResult();
                if (!codeCheckResult.isEmpty()) {
                    codeCheckResult.forEach((k, v) -> {
                        if (Objects.nonNull(v) && mrUrl.equals(k)) {
                            v.setResult(resultForCurrentPrOfInner);
                        }
                    });
                    ciCodeCheck.setTotalResult(resultForCurrentPrOfInner);
                    codeCheckOperation.findAndReplacePRInnerResult(uuid, ciCodeCheck);
                }
            }
        }
    }


    private void doSynchronizedWithWrongCheckForInner(String uuid) {
        CICodeCheck ciCodeCheckByUuid = codeCheckOperation.getCICodeCheckByUuid(uuid);
        CICodeCheck replacedCICodeCheck = new CICodeCheck();
        if (Objects.nonNull(ciCodeCheckByUuid)) {
            BeanUtils.copyProperties(ciCodeCheckByUuid, replacedCICodeCheck);
            // 蓝区数据已完成才会回写黄区数据
            if (Objects.nonNull(replacedCICodeCheck.getCodeCheckResult())) {
                Map<String, CodeCheckResultVo> codeCheckResult = replacedCICodeCheck.getCodeCheckResult();
                if (Objects.nonNull(codeCheckResult) && !codeCheckResult.isEmpty()) {
                    codeCheckResult.forEach((k, v) -> {
                        if (Objects.nonNull(v)) {
                            v.setResult(incResultSummaryOperation.getByUrlAndUUid(k, uuid).getResult());
                        }
                    });
                    codeCheckOperation.findAndReplacePRInnerResult(uuid, replacedCICodeCheck);
                }
            }
        }
    }

    /**
     * 按标准同步
     */
    private void checkCurrentPRUnderStandard(Map<String, List<CodeCheckDevVo>> subCodeCheckList,
                                             List<CodeCheckDevVo> VA, int countPRCheckInfo, Set<String> innerCodeCheckSet,
                                             String HK, String prUrl,
                                             CodeCheckDevCloud codeCheckDevCloud, CodeCheckInfo finalCodeCheckInfo) {
        String resultForCurrentPrOfInner;
        if (!subCodeCheckList.isEmpty() && CollectionUtils.isNotEmpty(VA) && countPRCheckInfo > 0) {
            // if (reposForInner.contains())
            VA.forEach(currentInnerCheckItem -> {
                // 告警状态略过
                if (!currentInnerCheckItem.equals(AssistantConstants.WARNING)) {
                    innerCodeCheckSet.add(currentInnerCheckItem.getResult());
                }
            });
            if (innerCodeCheckSet.size() == 1 && innerCodeCheckSet.contains(PASS)) {
                resultForCurrentPrOfInner = PASS;
            } else {
                resultForCurrentPrOfInner = AssistantConstants.YELLOW_CODECHECK_NO_PASS;
            }

        } else {
            resultForCurrentPrOfInner = AssistantConstants.YELLOW_CODECHECK_ERROR;
        }
        doSynchronizedWithAll(HK, prUrl, codeCheckDevCloud, resultForCurrentPrOfInner, finalCodeCheckInfo);
    }

    /**
     * 按外部同步
     */
    private void doSynchronizedWithAll(String currentPr, String prUrl, CodeCheckDevCloud codeCheckDevCloud,
                                       String resultForCurrentPrOfInner, CodeCheckInfo codeCheckInfo) {
        CodeCheckResultSummaryVo codeCheckResultSummaryVo = syncTotalResultWithInnerAndOutSideForCurrentPR(prUrl, codeCheckDevCloud.getUuid());
        if (Objects.nonNull(codeCheckInfo)) {
            if (Objects.nonNull(codeCheckResultSummaryVo)) {
                String outSideResult = codeCheckResultSummaryVo.getResult();
                if (codeCheckResultSummaryVo.getCodeCheckStatus().equals(NO_CHECK)) {
                    outSideResult = PASS;
                }
                // 融合结果
                String totalResult;
                totalResult = doJudgmentForCurrentPRWithInnerAndOutSide(resultForCurrentPrOfInner, outSideResult);
                if (outSideResult.equals(PASS) && resultForCurrentPrOfInner.equals(AssistantConstants.YELLOW_CODECHECK_ERROR)) {
                    // 外部通过黄区 error
                    totalResult = doJudgmentForCurrentPRWithInnerAndOutSide(AssistantConstants.YELLOW_CODECHECK_ERROR, outSideResult);
                }
                syncCodeCheckTotalResultBack(totalResult, codeCheckDevCloud.getUuid(), prUrl, currentPr);
            }
        } else {
            logger.info("----- current pr has no configuration for codecheck, and PR is: {} ----", prUrl);
            syncCodeCheckTotalResultBack(resultForCurrentPrOfInner, codeCheckDevCloud.getUuid(), prUrl, currentPr);
        }
    }

    private CodeCheckResultSummaryVo syncTotalResultWithInnerAndOutSideForCurrentPR(String prUrl, String uuid) {
        CodeCheckResultSummaryVo summaryVoByUuidAndPR = incResultSummaryOperation.getByUrlAndUUid(prUrl, uuid);
        if (Objects.nonNull(summaryVoByUuidAndPR)) {
            return summaryVoByUuidAndPR;
        }
        return null;
    }

    private void syncCodeCheckTotalResultBack(String totalResultForCurrentPR, String uuid, String prUrl, String currentPr) {
        fedBackCodeCheckTotalResultForPR(uuid, totalResultForCurrentPR, prUrl, currentPr);
    }

    /**
     * 同步单个PR的总结果
     */
    private void fedBackCodeCheckTotalResultForPR(String uuid, String totalResultForCurrentPR, String prUrl, String currentPr) {
        CICodeCheck ciCodeCheckByUuid = codeCheckOperation.getCICodeCheckByUuid(uuid);
        // 保证数据的有效性
        if (Objects.nonNull(ciCodeCheckByUuid)) {
            CICodeCheck replacedOne = new CICodeCheck();
            BeanUtils.copyProperties(ciCodeCheckByUuid, replacedOne);
            // 蓝区数据已同步完成
            if (Objects.nonNull(replacedOne.getCodeCheckResult())) {
                Map<String, CodeCheckResultVo> codeCheckResult = replacedOne.getCodeCheckResult();
                // 蓝区已正常回写
                if (!codeCheckResult.isEmpty()) {
                    codeCheckResult.forEach((k, v) -> {
                        // 对应的pr设置结果
                        if (k.equals(currentPr)) {
                            v.setResult(totalResultForCurrentPR);
                        }
                    });
                    codeCheckOperation.findAndReplacePRInnerResult(uuid, replacedOne);
                }
            }
        }
    }

    /**
     * 融合结果
     */
    private String doJudgmentForCurrentPRWithInnerAndOutSide(String resultForCurrentPrOfInner, String outSideResult) {
        String totalResultForCurrentPR = null;
        if (outSideResult.equals(PASS) && resultForCurrentPrOfInner.equals(PASS)) {
            totalResultForCurrentPR = PASS;
        } else {
            // 优先判断蓝区的总结果
            if (outSideResult.equals(NO_PASS)) {
                totalResultForCurrentPR = outSideResult;
                // 如果当前任务外部结果为失败
            } else {
                totalResultForCurrentPR = resultForCurrentPrOfInner;
            }
        }
        return totalResultForCurrentPR;
    }


    /**
     * 触发静态检查，黄区codecheck为空
     */
    private void doSyncDevError(String uuid, List<String> reposForInner, List<CodeCheckResultSummaryVo> incResultSummaryList) {
        CICodeCheck ciCodeCheckByUuid = codeCheckOperation.getCICodeCheckByUuid(uuid);
        // 静态检查的结果正常
        if (Objects.nonNull(ciCodeCheckByUuid) && Objects.nonNull(ciCodeCheckByUuid.getCodeCheckResult())) {
            Map<String, CodeCheckResultVo> codeCheckResult = ciCodeCheckByUuid.getCodeCheckResult();
            if (!codeCheckResult.isEmpty()) {
                if (CollectionUtils.isNotEmpty(incResultSummaryList)) {
                    incResultSummaryList.forEach(incEvent -> {
                        // pr对应的gitUrl
                        String gitUrl = incEvent.getMrUrl().substring(0, incEvent.getMrUrl().indexOf("/pulls")) + "git";
                        codeCheckResult.forEach((key, value) -> {
                            // 是内部pr && 对应的pr也有
                            if (reposForInner.contains(gitUrl) && incEvent.getMrUrl().replace(".", "%2e").equals(key)) {
                                value.setResult(AssistantConstants.YELLOW_CODECHECK_ERROR);
                            }
                        });
                    });
                }
            }
        }
        // 将 触发codecheck 但是 黄区失败的 设为 error，非黄区的不用管
        codeCheckOperation.findAndReplacePRInnerResult(uuid, ciCodeCheckByUuid);
    }


    /**
     * 保证时间有效
     */
    public Long checkTimeExist(CodeCheckDevCloud devCloudByUuid, long dateCreate, EventModel eventModel) {
        if (Objects.nonNull(devCloudByUuid.getRunning())) {
            if (Objects.nonNull(timeUtils.linkedStrToDate(devCloudByUuid.getRunning().getTimestamp()))) {
                dateCreate = timeUtils.linkedStrToDate(devCloudByUuid.getRunning().getTimestamp()).getTime();
            }
        } else {
            dateCreate = timeUtils.linkedStrToDate(eventModel.getTimestamp()).getTime();
        }
        return dateCreate;
    }


    /**
     * @param str URL 其他编码入参
     * @return afterDecode
     * @Description URL编码格式还原UTF-8
     */
    public String convertEncodingFormat(String str) {
        String decode = "";
        String afterDecode = "";
        try {
            // 二次解码
            decode = URLDecoder.decode(str, "UTF-8");
            afterDecode = URLDecoder.decode(decode, "UTF-8");
        } catch (Exception e) {
            logger.error("Decoding URL Exceptions caused by :" + e);
        }
        return afterDecode;
    }

    /**
     * 返回当前PR链接
     *
     * @param currentPr pr
     */
    private String dealWithCurrentPR(String currentPr) {
        String gitUrl = null;
        if (StringUtils.isNotBlank(currentPr)) {
            String prmUrl = currentPr.replace("%2e", ".");
            if (StringUtils.isNotBlank(prmUrl)) {
                int prefix = prmUrl.indexOf("/pulls");
                gitUrl = prmUrl.substring(0, prefix) + OHOS_REPO_INFO_SUFFIX;
            }
        }
        return gitUrl;
    }

    /**
     * 定时同步 增量任务数据到数据库中
     *
     * @return Response
     * @since 2022/11/9
     */
    @Override
    public Response getEventTasksBeforeSyncEventData() {
        logger.info("------  start to sync inc info ----------- ");
        // 获取当前的时间按照日期来进行分表,获取当前天数
        Calendar calendar = Calendar.getInstance();
        int dayOfMonth = calendar.get(Calendar.DAY_OF_MONTH);
        Response<Object> response = new Response<>();
        // 同步状态码为  0, -1, -2 CodeCheckCreateAccess状态的增量任务
        List<CodeCheckTaskIncVo> taskIncVos = taskIncOperation.getTaskByStatus(CodeCheckAccessConstant.CodeCheckCreateAccess);
        for (CodeCheckTaskIncVo codeCheckTaskIncVo : taskIncVos) {
            createEventSyncTask(codeCheckTaskIncVo, dayOfMonth);
        }
        response.setCode(String.valueOf(200));
        response.setMessage(CodeCheckAccessConstant.SUCCESS);
        return response;
    }

    /**
     * 定时检查当前已经创建了的代码检查任务
     * 将符合需要的状态码的任务同步入库
     *
     * @param codeCheckTaskIncVo 增量任务详情入参
     * @since 2022/11/9
     */
    private void createEventSyncTask(CodeCheckTaskIncVo codeCheckTaskIncVo, int dayOfMonth) {
        logger.info("----- inc sync start, uuid is: {}", codeCheckTaskIncVo.getUuid());
        incTaskExecutor.execute(() -> {
            if (codeCheckTaskIncVo.getProcessing() == 0) {
                int taskProgressStatus = restCodeCheckUtils.getTaskProgress(codeCheckTaskIncVo.getTaskId(),
                        CodeCheckAccessConstant.REGION);
                // 0表示正在执行中，还没执行完，跳过这一条   -2表示接口调用正常但是返回的task_status为null 重试后应该能成功
                // 1表示检查失败, 2表示检查成功, 3表示任务中止，都认为成已检查完成，但只有2检查成功才把结果入库。
                if (!(taskProgressStatus == 0 || taskProgressStatus == -2)) {
                    saveIncCheckData(codeCheckTaskIncVo, CodeCheckStatus.get(taskProgressStatus), dayOfMonth);
                }
            } else {
                // 创建任务失败的情况直接回写结果至码云
                Map<String, Object> eventResult = getDevRedisLock(codeCheckTaskIncVo, null);
                if (Objects.nonNull(eventResult)) {
                    long endTimestamp = (long) eventResult.get("endTimestamp");
                    String codeCheckTotalStatus = eventResult.get("codeCheckTotalStatus").toString();
                    setEventCodeCheckEndData(codeCheckTaskIncVo.getUuid(), endTimestamp, codeCheckTotalStatus);
                }
            }
            logger.info("----- inc sync end, uuid is: {}", codeCheckTaskIncVo.getUuid());
        });

    }

    /**
     * 保存增量的检查结果
     *
     * @param task            增量检查任务详情
     * @param codeCheckStatus 任务状态
     */
    private void saveIncCheckData(CodeCheckTaskIncVo task, CodeCheckStatus codeCheckStatus, int dayOfMonth) {
        logger.info("---------------------------------- start save inc check data -------------------------------");
        // 查询codecheck任务检查 summary结果
        Optional<CodeCheckResultSummary> taskSummary = restCodeCheckUtils.getTaskSummary(task.getTaskId(),
                CodeCheckAccessConstant.REGION);
        //如果查询到sumamry,进行数据封装
        CodeCheckResultSummaryVo summaryVo = new CodeCheckResultSummaryVo();
        if (taskSummary.isPresent()) {
            BeanUtils.copyProperties(taskSummary.get(), summaryVo);
            summaryVo.setUuid(task.getUuid());
            // 只有codecheck检查成功才能进行问题数的判断
            String result = CodeCheckAccessConstant.FAILED;
            //
            if (codeCheckStatus.equals(CodeCheckStatus.SUCCESS)) {
                result = summaryVo.getIssueCount() > 0 ? CodeCheckAccessConstant.NO_PASS : CodeCheckAccessConstant.PASS;
            }
            // summaryVo 即是当前增量任务对应的检查概览详情
            // 根据问题数量确定当前华为云codecheck检查任务的成功与失败
            summaryVo.setResult(result);
            summaryVo.setCodeCheckStatusByEnum(codeCheckStatus);
            summaryVo.setMrUrl(task.getMrUrl());

            List<DefectVo> defectList = new ArrayList<>();
            int solveCount = summaryVo.getSolve_count();
            if (solveCount >= CodeCheckConstants.MAX_SOLVE) {
                summaryVo.setSolve_count(-solveCount);
            }

            // 保存当前增量任务的summary概览数据
            incResultSummaryOperation.save(summaryVo);

            if (codeCheckStatus.equals(CodeCheckStatus.SUCCESS)) {
                // 一旦开始同步任务 修改数据同步状态为 1，表示正在同步中，让后续的定时任务不操作当前任务。
                taskIncOperation.updateProcessById(task.getId(), SyncStatus.SYNCING);
                // 同步details的结果
                logger.info("------------------------- start save inc check data detail -----------------------");
                // 按照问题数量来同步对应类型的问题到code_check_details表
                if (solveCount < CodeCheckConstants.MAX_SOLVE) {
                    logger.info("task's solve count < 1w , taskId : {}", task.getTaskId());
                    syncIncCheckDetails(task, defectList, summaryVo, CodeCheckConstants.STATUS_ALL, dayOfMonth);
                } else {
                    logger.info("task's solve count >= 1w , taskId : {}", task.getTaskId());
                    syncIncCheckDetails(task, defectList, summaryVo, CodeCheckConstants.STATUS_DEFAULT, dayOfMonth);
                    syncIncCheckDetails(task, defectList, summaryVo, CodeCheckConstants.STATUS_IGNORE, dayOfMonth);
                }

                logger.info("-------------------------- end save save inc check detail and success --------------");
            } else {
                taskIncOperation.updateProcessById(task.getId(), SyncStatus.SYNC_SUCCESS);
            }

            // 同步完成之后创建新的回写对象，将当前合入请求的增量任务总结果回写 --> 码云codecheck
            // codeCheckResultVo  当前增量任务的检查状态简单的相关信息
            CodeCheckResultVo codeCheckResultVo = new CodeCheckResultVo();
            // 当前pr的信息
            String mrUrl = summaryVo.getMrUrl();
            // 当前PR执行结果(success/failed)存入到 码云codecheck表 pr的状态
            codeCheckResultVo.setResult(summaryVo.getResult());
            codeCheckResultVo.setTaskId(task.getTaskId());

            EventModel event = eventCodeCheckOperation.getEventByUuid(task.getUuid());
            if (Objects.nonNull(event)) {
                // ark-x项目 则使用ark-x项目的地址
                codeCheckResultVo.setReportUrl(APPLICATION_URL + "/event/" + event.getId());
            }
            String codeCheckTotalStatus;

            // 社区增量任务状态同步完成。开始回写码云codecheck表相关数据
            Map<String, Object> eventResult = getDevRedisLock(task, codeCheckResultVo);
            logger.info("---------------------------------- sync community codecheck success ---------------------------------");

            // 回写同步结束后判断是否已回写完毕，完毕则填写码云总结果
            if (Objects.nonNull(eventResult)) {
                long endTimestamp = (long) eventResult.get("endTimestamp");
                codeCheckTotalStatus = eventResult.get("codeCheckTotalStatus").toString();
                setEventCodeCheckEndData(task.getUuid(), endTimestamp, codeCheckTotalStatus);
            }
        } else {
            taskIncOperation.updateProcessById(task.getId(), SyncStatus.SYNC_FAILED);
        }
        logger.info("---------------------------------- end save inc check data -------------------------------");
    }


    private void setEventCodeCheckEndData(String uuid, long endTimestamp, String codeCheckTotalStatus) {
        String key = RedisConstant.EVENT_LOCK_PREFIX + uuid;
        try {
            // 根据uuid获取redis的回写锁
            String lock = RedisConstant.LOCK;
            if (!redisOperateUtils.setIfAbsent(key, lock, 30, TimeUnit.MINUTES)) {
                while (RedisConstant.LOCK.equalsIgnoreCase(lock)) {
                    lock = redisOperateUtils.getAndSet(key, lock).toString();
                }
            }
            // 将codecheck总结果的结束时间，状态，耗时写入event表
            updateEventCodeCheckInfo(uuid, endTimestamp, codeCheckTotalStatus);
        } catch (ParseException e) {
            logger.error("----- update incDevCloud error:parse time,uuid is: {}", uuid);
        } finally {
            redisOperateUtils.set(key, RedisConstant.UNLOCK);
        }
    }


    /**
     * 查询增量问题详情，并且对问题进行同步处理
     *
     * @param task       任务详情
     * @param defectList 问题列表
     * @return List<DefectVo> 问题集合
     */
    private void syncIncCheckDetails(CodeCheckTaskIncVo task, List<DefectVo> defectList,
                                     CodeCheckResultSummaryVo summary, int status,int dayOfMonth) {
        logger.info("---------------------------- start get inc check data detail -----------------------------");
        int limit = 100;
        int offset = 0;
        Optional<CodeCheckResultDetails> details =
                restCodeCheckUtils.getTaskDetails(task.getTaskId(), offset, limit, CodeCheckAccessConstant.REGION, status);
        // 调用回去详情接口报错
        if (!details.isPresent()) {
            taskIncOperation.updateProcessById(task.getId(), SyncStatus.SYNC_FAILED);
            // 更新summary 的状态为Runtime Error：同步异常
            incResultSummaryOperation.updateCodeCheckStatus(summary.getUuid(),
                    summary.getTaskId(), CodeCheckStatus.ERROR);
            return;
        }
        // 同步数据
        int total = details.get().getTotal();
        if (total == 0) {
            boolean judge = taskIncOperation.updateSuccess(task.getId());
            if (!judge) {
                logger.info("taskId : {}  , it has failed, now status is {}", task.getTaskId(), status);
            }
            return;
        }
        // 回显的数据只回显前100条
        defectList.addAll(getIncDefectVosFromResponse(details.get().getDefects(), task, summary));
        while (offset < total) {
            Integer temp = offset;
            // 先全部使用线程池同步 后续在看需不需根据数据量大小判断
            syncIncThreadPool.execute(() -> {
                long sUid = uuidSnowFlakeUtil.nextId();
                Optional<CodeCheckResultDetails> taskDetails = restCodeCheckUtils
                        .getTaskDetails(task.getTaskId(), temp, limit, CodeCheckAccessConstant.REGION, status);
                // 正常同步
                if (taskDetails.isPresent()) {
                    CodeCheckResultDetails codeCheckResultDetails = taskDetails.get();
                    if (CollectionUtils.isNotEmpty(codeCheckResultDetails.getDefects())) {
                        saveDefectVos(codeCheckResultDetails.getDefects(), task, summary, sUid, dayOfMonth);
                        taskIncOperation.updateSuccess(task.getId());
                    }
                } else {
                    incResultDetailsOperation.remove(summary.getUuid(), summary.getTaskId());
                    taskIncOperation.updateProcessById(task.getId(), SyncStatus.SYNC_FAILED);
                    // 更新summary 的状态为Error
                    incResultSummaryOperation
                            .updateCodeCheckStatus(summary.getUuid(), summary.getTaskId(), CodeCheckStatus.ERROR);
                }
            });
            offset += limit;
        }
        logger.info("---------------------------------- end get inc check data detail -----------------------------");
    }

    /**
     * 增量问题详细数据封装
     *
     * @param defects 问题数据
     * @return List<DefectVo>
     */
    private List<DefectVo> getIncDefectVosFromResponse(List<CodeCheckIssueDefect> defects,
                                                       CodeCheckTaskIncVo task, CodeCheckResultSummaryVo summary) {
        // 从响应的数据中解析问题vo
        List<DefectVo> defectVos = new ArrayList<>(defects.size());
        defects.forEach(defect -> {
            DefectVo defectVo = new DefectVo();
            defectVo.setTaskId(task.getTaskId());
            defectVo.setUuid(task.getUuid());
            defectVo.setSubCodeCheckName("codeCheck");
            defectVo.setResult(summary.getResult());
            defectVo.setFilepath(defect.getFilePath());
            defectVo.setLineNumber(defect.getLineNumber());
            defectVo.setRuleName(defect.getDefectContent());
            defectVo.setDefectContent(defect.getDefectContent());
            defectVo.setDefectLevel(defect.getDefectLevel());
            defectVo.setRuleSystemTags(defect.getRuleSystemTags());
            defectVo.setFileName(defect.getFilePath());
            defectVo.setCreatedAt(defect.getCreatedAt());
            defectVo.setDefectId(defect.getDefectId());
            defectVo.setIssueKey(defect.getIssueKey());
            defectVo.setDefectCheckerName(defect.getDefectCheckerName());
            if (!defect.getDefectCheckerName().contains("FossScan")) {
                // 码云检查的暂时没有代码片段
                defectVo.setFragment(defect.getFragment());
                defectVo.setDefectStatus(defect.getDefectStatus());
                defectVos.add(defectVo);
            }
        });
        return defectVos;
    }


    private Map<String, Object> getDevRedisLock(CodeCheckTaskIncVo task, CodeCheckResultVo codeCheckResultVo) {
        String devKey = RedisConstant.DEV_LOCK_PREFIX + task.getUuid();
        try {
            // 根据uuid获取 码云codecheck回写锁
            String devLock = RedisConstant.LOCK;
            if (!redisOperateUtils.setIfAbsent(devKey, devLock, 20, TimeUnit.MINUTES)) {
                while (RedisConstant.LOCK.equalsIgnoreCase(devLock)) {
                    devLock = redisOperateUtils.getAndSet(devKey, devLock).toString();
                }
            }
            return saveCICodeCheckResult(task, codeCheckResultVo);
        } finally {
            redisOperateUtils.set(devKey, RedisConstant.UNLOCK);
        }
    }

    /**
     * 将增量任务执行状态从summary结果 同步 到codecheck更新任务状态
     *
     * @param codeCheckResultVo 增量PR任务
     */
    private Map<String, Object> saveCICodeCheckResult(CodeCheckTaskIncVo task, CodeCheckResultVo codeCheckResultVo) {
        Map<String, Object> eventResult = null;
        String uuid = task.getUuid();
        String taskId = task.getTaskId();
        logger.info("----- update incCICodeCheck start, uuid is: {}, taskId is: {}", uuid, taskId);
        // 获取码云 codecheck表， 更新任务状态
        CICodeCheck ciCodeCheck = codeCheckOperation.getCICodeCheckByUuid(uuid);

        if (Objects.nonNull(ciCodeCheck)) {
            // 判断event的开始执行时间是否因某种原因超过1小时，如果是则设置为异常状态，不再同步
            if (checkCICodeCheckState(ciCodeCheck, task.getId())) {
                return null;
            }
            // 如果codeCheckResultVo不为空，则码云回写数据有新增的需要入库
            if (Objects.nonNull(codeCheckResultVo)) {
                Map<String, CodeCheckResultVo> ciCodeCheckResult = ciCodeCheck.getCodeCheckResult();
                // 如果码云ciCodeCheckResult 字段回写数据为空，设置为空
                if (Objects.isNull(ciCodeCheckResult)) {
                    ciCodeCheckResult = new HashMap<>();
                    // 将summary 取出来的对应当前增量任务的状态数据 替换到码云codecheck表中
                    ciCodeCheck.setCodeCheckResult(ciCodeCheckResult);
                }
                String mrKey = task.getMrUrl().replace(".", "%2e");
                // 衔接  ciCodeCheck.setCodeCheckResult(ciCodeCheckResult);
                ciCodeCheckResult.put(mrKey, codeCheckResultVo);
            }
            // 获取event的prMsg，通过prMsg判断全部PR数据是否都已入库，是则设置total结果
            eventResult = setCIEndTotalResult(task.getId(), ciCodeCheck);

            // 多个pr 合成一个codecheck.codeCheckResult字段HashMap 更新当前所有的PR详情
            codeCheckOperation.replaceCICodeCheckByUuid(ciCodeCheck);
            logger.info("----- update incCICodeCheck success,uuid is: {},taskId is: {}", uuid, taskId);
        } else {
            logger.error("----- update incCICodeCheck error : devData is null, uuid is: {}, taskId is: {}", uuid, taskId);
        }
        // 所有PR检查结果
        return eventResult;
    }

    /**
     * 更新所有PR 对应的状态信息
     */
    private boolean checkCICodeCheckState(CICodeCheck ciCodeCheck, String incTaskVoId) {
        String startTime = ciCodeCheck.getStartTime();
        try {
            long startTimestamp = second.parse(startTime).getTime();
            long nowTimestamp = System.currentTimeMillis();
            logger.info("checkCICodeCheckState nowTimestamp:{}", nowTimestamp);
            logger.info("checkCICodeCheckState startTimestamp:{}", startTimestamp);
            if (nowTimestamp - startTimestamp > TimeUtils.HOUR) {
                taskIncOperation.updateProcessById(incTaskVoId, SyncStatus.SYNC_TIME_OUT);
                // 更新门禁codecheck状态
                updateEventCodeCheckInfo(ciCodeCheck.getUuid(), nowTimestamp, CodeCheckAccessConstant.TIME_OUT);
                return true;
            }
        } catch (ParseException e) {
            logger.error("----- update ciCodeCheck error,uuid: {},msg is: {}", ciCodeCheck.getUuid(), e.getMessage());
        }
        return false;
    }

    /**
     * 在蓝区正常执行codecheck的情况下，对内部仓库的结果过滤出有效的黄区结果
     *
     * @param reposForInner    内部仓信息
     * @param subCodeCheckList 所有提交的pr信息
     */
    public String doFilterForInnerReposTotalResult(Map<String, List<CodeCheckDevVo>> subCodeCheckList,
                                                   List<String> reposForInner, CodeCheckInfo codeCheckInfo, List<CodeCheckResultSummaryVo> byUuid,
                                                   CodeCheckDevCloud codeCheckDevCloud) {
        String innerTotalResult = null;
        HashSet<String> setFinalResult = new HashSet<>();
        // 判断方向由 蓝区->黄区 不可逆向
        for (CodeCheckResultSummaryVo codeCheckResultSummaryVo : byUuid) {
            String mrUrl = codeCheckResultSummaryVo.getMrUrl();
            String devUrl = mrUrl.replace(".", "%2e");
            String checkIfTSC = mrUrl.substring(0, mrUrl.indexOf("/pulls"));
            String gitUrl = checkIfTSC + ".git";
            // 有效的黄区结果
            if (reposForInner.contains(gitUrl) && Objects.nonNull(codeCheckInfo) &&
                    !checkIfTSC.equals(EXCLUDE_PROJECT_NAME) && !gitUrl.startsWith(ARK_PROJECT)) {
                // 只有黄区出最终结果的情况下才进行检索
                if (StringUtils.isNotBlank(codeCheckDevCloud.getTotalResult())) {
                    if (subCodeCheckList.isEmpty()) {
                        return FAILED;
                    } else {
                        Iterator<String> iterator = subCodeCheckList.keySet().iterator();
                        while (iterator.hasNext()) {
                            String next = iterator.next();
                            if (devUrl.equals(next)) {
                                List<CodeCheckDevVo> devVoList = subCodeCheckList.get(devUrl);
                                if (CollectionUtils.isNotEmpty(devVoList)) {
                                    devVoList.forEach(devVo -> {
                                        // 警告只给提示
                                        if (!devVo.getResult().equals(AssistantConstants.WARNING)) {
                                            setFinalResult.add(devVo.getResult());
                                        }
                                    });
                                } else {
                                    // 单或多pr 的情况下,某条内部pr黄区检查项目 同步出现问题的情况下
                                    return FAILED;
                                }
                            }
                        }
                    }
                } else {
                    return RUNNING;
                }
            } else {
                setFinalResult.add(PASS);
            }
        }
        innerTotalResult = doJudgmentInnerTotalForCurrentPR(setFinalResult);
        return innerTotalResult;
    }

    /**
     * 判断当前黄区任务的总结果
     *
     * @param setFinalResult 所有的有效检查项的set集合
     * @return String
     */
    public String doJudgmentInnerTotalForCurrentPR(HashSet<String> setFinalResult) {
        if (setFinalResult.size() == 1 && setFinalResult.contains(PASS)) {
            return PASS;
        } else {
            return FAILED;
        }
    }

    /**
     * 当前合入请求有效的蓝区总结果
     *
     * @param byUuid 当前合入请求下的所有pr 蓝区结果
     * @return String
     */
    public String doFilterForCommunityCheckResult(List<CodeCheckResultSummaryVo> byUuid) {
        String totalResultForBCheck;
        Set<String> collect = new HashSet<>();
        byUuid.forEach(codeCheckResultSummaryVo -> collect.add(codeCheckResultSummaryVo.getResult()));
        // 未配置/未触发codecheck检查 的情况也已经默认通过
        if ((collect.contains(PASS) && collect.size() == 1) || CollectionUtils.isEmpty(collect)) {
            totalResultForBCheck = PASS;
        } else {
            totalResultForBCheck = FAILED;
        }
        return totalResultForBCheck;
    }

    /**
     * 超时 则更新ci_portal_event   CodeCheckInfo状态
     */
    private void updateEventCodeCheckInfo(String uuid, long endTimestamp, String codeCheckTotalStatus)
            throws ParseException {
        logger.info("updateEventCodeCheckInfo endTimestamp:{}", endTimestamp);
        EventModel oldEvent = eventCodeCheckOperation.getEventByUuid(uuid);
        if (Objects.nonNull(oldEvent)) {
            Object codeCheckInfo = oldEvent.getCodeCheckInfo();
            if (Objects.nonNull(codeCheckInfo)) {
                CodeCheckInfo codeCheckTotalResult =JsonUtils.fromJson(JsonUtils.toJson(codeCheckInfo), CodeCheckInfo.class);
                String endTime = DateFormatUtils.format(endTimestamp, DateEnums.SECOND_ONE.getPattern());
                codeCheckTotalResult.setCheckEndTime(endTime);
                long startTimestamp = secondOne.parse(codeCheckTotalResult.getCheckStartTime()).getTime();
                codeCheckTotalResult.setDuration(String.valueOf((endTimestamp - startTimestamp) / 60000));
                codeCheckTotalResult.setCodeCheckStatus(codeCheckTotalStatus);
                oldEvent.setCodeCheckInfo(codeCheckTotalResult);
                eventCodeCheckOperation.updateCodeCheckInfo(oldEvent);
            }
        }
    }


    /**
     * 获取所有的PR 检查结果
     *
     * @return: 检查结果是否存在
     */
    private Map<String, Object> setCIEndTotalResult(String incTaskVoId, CICodeCheck ciCodeCheck) {
        String uuid = ciCodeCheck.getUuid();
        EventModel event = eventCodeCheckOperation.getEventByUuid(uuid);
        // 任务存在
        if (Objects.nonNull(event) && CollectionUtils.isNotEmpty(event.getPrMsg())) {
            // 更新码云codecheck表的 pr任务状态
            Map<String, CodeCheckResultVo> codeCheckResult = ciCodeCheck.getCodeCheckResult();
            if (Objects.isNull(codeCheckResult)) {
                logger.error("----- update incCICodeCheck error:ciCodeCheckResult is null, uuid: {}", uuid);
                return null;
            }
            // 更新增量任务状态
            taskIncOperation.updateProcessById(incTaskVoId, SyncStatus.SYNC_SUCCESS);
            logger.info("----- update incCICodeCheck total result success, uuid: {}", uuid);

            // PR个数与已有数据PR个数不相等，则还未同步完成，不能设置总结果
            if (event.getPrMsg().size() != codeCheckResult.size()) {
                return null;
            }
            // 判断是否已写入码云回写总结果，未写入则写入
            // 判断summary任务状态是否为 end 非end则继续同步，end则表示codecheck已经完成，此时同步内部codecheck数据
            if (!CiConstants.CHECK_END.equalsIgnoreCase(ciCodeCheck.getCurrentStatus())) {
                return getCITotalResult(ciCodeCheck);
            }
        } else {
            // 任务不存在
            taskIncOperation.updateProcessById(incTaskVoId, SyncStatus.RUNNING);
            logger.info("----- update incCICodeCheck, event is running, uuid: {}", uuid);
        }
        return null;
    }


    /**
     * 根据PR的检查任务状态来确定回写码云codecheck表的最终结果
     * <p>
     * 这里的结果判断只包含了蓝区/ 静态检查总结果
     *
     * @param ciCodeCheck
     */
    private Map<String, Object> getCITotalResult(CICodeCheck ciCodeCheck) {
        // 社区pr codecheck相关检查项
        ciCodeCheck.setCurrentStatus(CiConstants.CHECK_END);

        long endTimestamp = System.currentTimeMillis();
        ciCodeCheck.setEndTime(DateFormatUtils.format(endTimestamp, DateEnums.SECOND.getPattern()));
        Set<String> resultSet = new HashSet<>();
        // 获取当前合入的所有PR相关信息
        Map<String, CodeCheckResultVo> codeCheckResult = ciCodeCheck.getCodeCheckResult();
        for (CodeCheckResultVo resultVos : codeCheckResult.values()) {
            if (Objects.isNull(resultVos)) {
                continue;
            }
            // 每个PR 对应的会将检查结果同步到summary ，CiCodeCheck将对应PR的summary检查结果合并到 码云codecheck表。
            // 获取所有的PR的检查结果 组成 set<String>
            String result = resultVos.getResult();
            if (CodeCheckAccessConstant.NOT_CONFIGURE.equalsIgnoreCase(result)) {
                resultSet.add(CodeCheckAccessConstant.NO_CHECK);
            } else if (PASS.equalsIgnoreCase(result)) {
                resultSet.add(PASS);
            } else if (CodeCheckAccessConstant.NO_PASS.equalsIgnoreCase(result)) {
                resultSet.add(CodeCheckAccessConstant.NO_PASS);
            } else {
                resultSet.add(PASS);
            }
        }

        // 设置码云回写总结果与codecheck门禁总结果（失败最高优先级，其次不通过，再通过，最后未配置）
        String codeCheckTotalStatus;
        if (resultSet.contains(CodeCheckConstants.FAILED)) {
            ciCodeCheck.setTotalResult(CodeCheckConstants.FAILED);
            codeCheckTotalStatus = CodeCheckConstants.FAILED;
        } else if (resultSet.contains(CodeCheckConstants.NO_PASS)) {
            ciCodeCheck.setTotalResult(CodeCheckConstants.FAILED);
            codeCheckTotalStatus = CodeCheckConstants.NO_PASS;
        } else if (resultSet.contains(CodeCheckConstants.PASS)) {
            ciCodeCheck.setTotalResult(CodeCheckConstants.PASS);
            codeCheckTotalStatus = CodeCheckConstants.SUCCESS;
        } else {
            ciCodeCheck.setTotalResult(CodeCheckConstants.PASS);
            codeCheckTotalStatus = CodeCheckConstants.NO_CHECK;
        }
        // 返回event门禁总结果
        Map<String, Object> eventResult = new HashMap<>();
        eventResult.put("endTimestamp", endTimestamp);
        eventResult.put("codeCheckTotalStatus", codeCheckTotalStatus);
        return eventResult;
    }


    /**
     * 避免重复同步的情况 利用upsert插入
     */
    private void saveDefectVos(List<CodeCheckIssueDefect> defects, CodeCheckTaskIncVo
            task, CodeCheckResultSummaryVo summary, long sUid, int dayOfMonth) {
        defects.forEach(defect -> {
            DefectVo defectVo = new DefectVo();
            // 同一个 taskId 对应相同的snowid
            // defectVo.setSnowid(sUid);
            defectVo.setTaskId(task.getTaskId());
            defectVo.setUuid(task.getUuid());
            defectVo.setResult(summary.getResult());
            defectVo.setFilepath(defect.getFilePath());
            defectVo.setLineNumber(defect.getLineNumber());
            // 码云检查的暂时没有代码片段
            defectVo.setFragment(defect.getFragment());
            defectVo.setRuleName(defect.getDefectContent());
            defectVo.setDefectCheckerName(defect.getDefectCheckerName());
            defectVo.setDefectContent(defect.getDefectContent());
            defectVo.setDefectLevel(defect.getDefectLevel());
            defectVo.setDefectStatus(defect.getDefectStatus());
            defectVo.setRuleSystemTags(defect.getRuleSystemTags());
            defectVo.setFileName(defect.getFilePath());
            defectVo.setCreatedAt(defect.getCreatedAt());
            defectVo.setDefectId(defect.getDefectId());
            if (defect.getDefectCheckerName().contains("FossScan")) {
                insertfossScanFragment(defect, task.getTaskId(), task.getUuid(), null);
            } else {
                // 代码检查详情问题入库
                incResultDetailsOperation.saveByUpsert(defectVo, dayOfMonth);
                //taskIncDetailMapper.saveByUpsert(defectVo);
            }
        });
    }

    /**
     * insertfossScanFragment
     * 将fosscan问题入库
     *
     * @param defect defect
     * @param taskId taskId
     * @param uuid   uuid
     * @param date   date
     * @since 2022/11/12
     */
    public void insertfossScanFragment(CodeCheckIssueDefect defect, String taskId, String uuid, String date) {
        List<CodeCheckIssueFragment> fragments = defect.getFragment();
        if (fragments == null || fragments.size() == 0) {
            logger.info("fossScanFragment is null");
            return;
        }
        String compress = fragments.get(0).getLineContent();
        try {
            String fragmentStr = CompressUtil.uncompress(compress);
            FossscanFragment fossscanFragment = JsonUtils.fromJson(fragmentStr, FossscanFragment.class);
            if (fossscanFragment != null) {
                //去除匹配行数小于10的匹配段
                for (FossScan foss : fossscanFragment.getScanResults()) {
                    foss.setHits(foss.getHits().stream().filter(hit -> hit.getHitEndLine() - hit.getHitStartLine() > 10)
                            .sorted((Comparator.comparingInt(hit -> hit.getHitStartLine() - hit.getEndLine())))
                            .collect(Collectors.toList()));
                }
                //去除没有匹配段的开源文件
                fossscanFragment.setScanResults(fossscanFragment.getScanResults()
                        .stream().filter(fossScan -> fossScan.getHits().size() > 0).collect(Collectors.toList()));
                String fullPath = defect.getFilePath();
                fossscanFragment.setDefectId(defect.getDefectId());
                fossscanFragment.setIssueKey(defect.getIssueKey());
                fossscanFragment.setTaskId(taskId);
                fossscanFragment.setUuid(uuid);
                fossscanFragment.setDate(date);
                if (defect.getDefectStatus().equals("0")) {
                    fossscanFragment.setConfirm(false);
                } else {
                    fossscanFragment.setConfirm(true);
                }
                if (StringUtils.isNotEmpty(fullPath)) {
                    int li = fullPath.lastIndexOf("/");
                    fossscanFragment.setPath(fullPath.substring(0, li));
                    fossscanFragment.setFileName(fullPath.substring(li + 1, fullPath.length()));
                    fossscanFragment.setSuffix(fossscanFragment.getFileName().split("\\.")[1]);
                }
                if (fossscanFragment.isConfirm()) {
                    FossscanFragment oldFoss = fossScanOperation.queryOneByKey(fossscanFragment.getIssueKey());
                    if (oldFoss != null) {
                        fossscanFragment.setConfirmTime(oldFoss.getConfirmTime());
                        fossscanFragment.setComponentName(oldFoss.getComponentName());
                        fossscanFragment.setComponentVersion(oldFoss.getComponentVersion());
                        fossscanFragment.setFossType(oldFoss.getFossType());
                        fossscanFragment.setRemarks(oldFoss.getRemarks());
                        fossscanFragment.setOpen(oldFoss.getOpen());
                        fossscanFragment.setUserId(oldFoss.getUserId());
                        fossscanFragment.setUserName(oldFoss.getUserName());
                    }
                }
                fossScanOperation.deleteFossFragment(fossscanFragment.getIssueKey(), uuid, date);
                fossScanOperation.insertFossFragment(fossscanFragment);
            }
        } catch (IOException e) {
            logger.error("fossScanFragment error");
            logger.error(e.getMessage());
        }
    }

}
