package com.huawei.operation;

import com.huawei.entity.vo.codecheck.codecheckdetails.CodeCheckTaskVo;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.core.query.Query;
import org.springframework.stereotype.Component;

@Component
public class TaskOperation {
    @Autowired
    @Qualifier("codeCheckMongoTemplate")
    private  MongoTemplate mongoTemplate;

    public CodeCheckTaskVo getTaskById(String taskId) {
        Query query = Query.query(Criteria.where("taskId").is(taskId));
        return mongoTemplate.findOne(query,CodeCheckTaskVo.class);
    }
}
