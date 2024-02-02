/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 */

package com.huawei.impl;

import com.huawei.ci.common.contant.ResponseCode;
import com.huawei.ci.common.pojo.vo.Response;
import com.huawei.entity.vo.projectmanagement.ProjectRuleVo;
import com.huawei.operation.RuleOperation;
import com.huawei.service.ProjectManagementService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Objects;

/**
 * @Description: ProjectManagementServiceImpl
 * @ClassName: com.huawei.service
 * @since : 2022/11/10  10:38
 **/
@Component
public class ProjectManagementServiceImpl implements ProjectManagementService {
    @Autowired
    private RuleOperation ruleOperation;

    /**
     * 根据规则名称查询规则集
     *
     * @param ruleName 规则名称
     * @return Response
     */
    @Override
    public Response selectByRuleName(String ruleName) {
        List<ProjectRuleVo> projectRuleVos = ruleOperation.selectByRuleName(ruleName);
        // 查询不到数据
        if (Objects.isNull(projectRuleVos) && projectRuleVos.size() == 0) {
            return Response.result(ResponseCode.FILED, "The rules are wrong");
        }
        return Response.result(ResponseCode.SUCCESS, projectRuleVos.get(0));
    }
}
