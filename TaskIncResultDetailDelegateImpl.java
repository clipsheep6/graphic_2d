package com.huawei.impl;

import com.huawei.ci.common.pojo.vo.Response;
import com.huawei.entity.pojo.Defect;
import com.huawei.mapper.TaskIncDetailMapper;
import com.huawei.service.TaskIncResultDetailService;
import com.huawei.utils.UUIDSnowFlakeUtil;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * @Description 代码详情业务
 * @ClassName com.huawei.impl
 * @since 2022/11/14  09:22
 **/
@Component
public class TaskIncResultDetailDelegateImpl implements TaskIncResultDetailService {
    // 日志
    private static final Logger logger = LoggerFactory.getLogger(TaskIncResultDetailDelegateImpl.class);

    @Autowired
    private TaskIncDetailMapper taskIncDetailMapper;

    @Autowired
    private UUIDSnowFlakeUtil uuidSnowFlakeUtil;


    /**
     * 当前测试数据，后期进行修改
     */
    @Override
    public Response insertData() {
        List<Defect> defectVoList = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            Defect defectVo = new Defect();
            long l = uuidSnowFlakeUtil.nextId();
            defectVo.setTaskId(String.valueOf(i));
            defectVo.setIssueKey("testKey");
            defectVo.setFilepath("test");
            defectVo.setRuleName("testRuleName");
            defectVo.setDefectContent("test");
            defectVo.setDefectCheckerName("testCheckerName");
            defectVo.setLineNumber(0);
            defectVo.setDefectLevel(2);
            defectVo.setDefectStatus(1);
            defectVo.setDate("testDateNow");
            defectVo.setFileName("testName");
            defectVo.setRuleSystemTags("testTags");
            defectVo.setCreatedTime(String.valueOf(new Date().getTime()));
            defectVo.setUpdateTime(String.valueOf(new Date().getTime()));
            defectVo.setUpdateUser("testUser");
            defectVo.setDefectId("defectId");
            defectVo.setUuid(String.valueOf("testUUid"));
            defectVo.setSnowid(l);
            defectVoList.add(defectVo);
        }

        taskIncDetailMapper.insertIntoDetails(defectVoList);
        logger.info("insert finished");
        return null;
    }

    @Override
    public List<Defect> getAllData() {
        List<Defect> allData = taskIncDetailMapper.getAllData();
        return allData;
    }
}
